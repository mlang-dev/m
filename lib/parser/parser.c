/*
 * parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a parser's main driver, taking a parsing table & rule and parse text 
 * into ast according to the parsing table and rule set
 */
#include "parser/parser.h"
#include "parser/m_parsing_table.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include "error/error.h"
#include <assert.h>

struct parser *_parser_new(parsing_table *pt, parsing_rules *pr, parsing_symbols *psd, parsing_states *pstd)
{
    struct parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->stack_top = 0;
    parser->pt = pt;
    parser->pr = pr;
    parser->psd = psd;
    parser->pstd = pstd;
    hashtable_init_with_value_size(&parser->symbol_2_int_types, sizeof(int), 0);
    for (int i = 0; i < TYPE_TYPES; i++) {
        hashtable_set_int(&parser->symbol_2_int_types, get_type_symbol(i), i);
    }
    return parser;
}

struct parser *parser_new()
{
    return _parser_new(&m_parsing_table, &m_parsing_rules, &m_parsing_symbols, &m_parsing_states);
}

void parser_free(struct parser *parser)
{
    hashtable_deinit(&parser->symbol_2_int_types);
    FREE(parser);
}

void _push_state(struct parser *parser, u16 state, struct ast_node *ast)
{
    struct stack_item *si = &parser->stack[parser->stack_top++];
    si->state_index = state;
    si->ast = ast;
}

struct stack_item *_pop_state(struct parser *parser)
{
    return &parser->stack[--parser->stack_top];
}

struct stack_item *_get_top_state(struct parser *parser)
{
    return &parser->stack[parser->stack_top-1];
}

struct stack_item *_get_start_item(struct parser *parser, u8 symbol_count)
{
    return &parser->stack[parser->stack_top-symbol_count];
}

void _pop_states(struct parser *parser, u8 symbol_count)
{
    assert(parser->stack_top >= symbol_count);
    parser->stack_top -= symbol_count;
}

struct ast_node *_build_terminal_ast(struct token *tok)
{
    enum node_type node_type;
    struct ast_node *ast = 0;
    switch(tok->token_type){
        default:
            node_type = tok->token_type << 16;
            ast = ast_node_new(node_type, 0, 0, false, tok->loc);
            break;
        case TOKEN_EOF:
            ast = ast_node_new(NULL_NODE, 0, 0, false, tok->loc);
            break;
        case TOKEN_UNIT:
            ast = ast_node_new(UNIT_NODE, TYPE_UNIT, get_type_symbol(TYPE_UNIT), false, tok->loc);
            break;
        case TOKEN_OP:
            //*hacky way to transfer opcode
            node_type = (tok->token_type << 16) | tok->opcode;
            ast = ast_node_new(node_type, 0, 0, false, tok->loc);
            break;
        case TOKEN_IDENT:
            ast = ident_node_new(tok->symbol_val, tok->loc);
            break;
        case TOKEN_INT:
            ast = int_node_new(tok->int_val, tok->loc);
            break;
        case TOKEN_DOUBLE:
            ast = double_node_new(tok->double_val,tok->loc);
            break;
        case TOKEN_TRUE:
            ast = bool_node_new(true, tok->loc);
            break;
        case TOKEN_FALSE:
            ast = bool_node_new(false, tok->loc);
            break;
        case TOKEN_CHAR:
            ast = char_node_new(tok->int_val, tok->loc);
            break;
        case TOKEN_STRING:
            ast = string_node_new(string_get(tok->str_val), tok->loc);
            break;
        }
    return ast;
}
struct ast_node *_wrap_as_block_node(struct ast_node *node)
{
    struct array nodes;
    array_init(&nodes, sizeof(struct ast_node *));
    array_push(&nodes, &node);
    return block_node_new(&nodes);
}

struct ast_node *_build_nonterm_ast(struct hashtable *symbol_2_int_types, struct parse_rule *rule, struct stack_item *items)
{
    enum op_code opcode;
    struct ast_node *ast = 0;
    struct ast_node *node = 0;
    struct ast_node *node1 = 0;
    struct ast_node *node2 = 0;
    struct ast_node *node3 = 0;
    struct ast_node *node4 = 0;
    bool is_variadic = false;
    bool is_ref_annotated = false;
    symbol type_name = 0;
    if (!rule->action.node_type){
        if (rule->action.item_index_count == 0){
            return items[0].ast;
        }else{
            return items[rule->action.item_index[0]].ast;
        }
    }
    switch (rule->action.node_type) {
    case NULL_NODE:
    case TOTAL_NODE:
    case UNIT_NODE:
    case LITERAL_NODE:
    case IDENT_NODE:
        printf("type: %d is not supported for nonterm node.", rule->action.node_type);
        break;
    case IMPORT_NODE:
        node = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        ast = import_node_new(node->ident->name, node1, node->loc);
        break;
    case MEMORY_NODE:
        node = items[rule->action.item_index[0]].ast;
        if (rule->action.item_index_count==2){
            node1 = items[rule->action.item_index[1]].ast;
        }
        ast = memory_node_new(node, node1, node->loc);
        break;
    case UNARY_NODE:
        assert(rule->action.item_index_count==2);
        node = items[rule->action.item_index[0]].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[rule->action.item_index[1]].ast;
        ast = unary_node_new(opcode, node, rule->action.item_index[0] > rule->action.item_index[1], node->loc);
        break;
    case VAR_NODE:
        node = items[rule->action.item_index[1]].ast;
        assert(node->node_type == IDENT_NODE);
        if (rule->action.item_index[0]) {
            node1 = items[rule->action.item_index[2]].ast;
            if (rule->action.item_index_count == 4) {
                //has type and has init value
                assert(node1->node_type == IDENT_NODE||node1->node_type == UNARY_NODE);
                if(node1->node_type == IDENT_NODE){
                    type_name = node1->ident->name;
                }else{
                    assert(node1->unop->opcode == OP_BAND);
                    assert(node1->unop->operand->node_type == IDENT_NODE);
                    type_name = node1->unop->operand->ident->name;
                    is_ref_annotated = true;
                }
                node2 = items[rule->action.item_index[3]].ast;
                ast = var_node_new2(node->ident->name, type_name, is_ref_annotated, node2, false, node->loc);
            } else { // has no type info, has init value
                ast = var_node_new2(node->ident->name, 0, false, node1, false, node->loc);
            }
        } else if (rule->action.item_index_count > 2) {
            //just has ID and type
            node1 = items[rule->action.item_index[2]].ast;
            assert(node1->node_type == IDENT_NODE||node1->node_type == UNARY_NODE);
            if(node1->node_type == IDENT_NODE){
                type_name = node1->ident->name;
            }else{
                assert(node1->unop->opcode == OP_BAND);
                assert(node1->unop->operand->node_type == IDENT_NODE);
                type_name = node1->unop->operand->ident->name;
                is_ref_annotated = true;
            }
            ast = var_node_new2(node->ident->name, type_name, is_ref_annotated, 0, false, node->loc);
        } else {
            //just ID
            ast = var_node_new2(node->ident->name, 0, false, 0, false, node->loc);
        }
        break;
    case RANGE_NODE:
        node1 = items[rule->action.item_index[0]].ast; //start
        if(rule->action.item_index_count == 3){
            node2 = items[rule->action.item_index[1]].ast; //step
            node3 = items[rule->action.item_index[2]].ast; // end
        } else {
            node2 = 0;
            node3 = items[rule->action.item_index[1]].ast; // end
        }
        ast = range_node_new(node1, node3, node2, node1->loc);
        break;
    case FOR_NODE:
        node = items[rule->action.item_index[0]].ast;
        assert(node->node_type == VAR_NODE);
        node1 = items[rule->action.item_index[1]].ast; //range
        node2 = items[rule->action.item_index[2]].ast; //body
        node1->range->end = binary_node_new(OP_LT, ident_node_new(node->var->var_name, node->loc), node1->range->end, node1->range->end->loc);
        ast = for_node_new(node, node1, node2, node->loc);
        break;
    case IF_NODE:
        node = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        if (rule->action.item_index_count == 3){
            node2 = items[rule->action.item_index[2]].ast;
        }
        ast = if_node_new(node, node1, node2, node->loc);
        break;
    case BINARY_NODE:
        node = items[rule->action.item_index[1]].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[2]].ast;
        ast = binary_node_new(opcode, node, node1, node->loc);
        break;
    case MEMBER_INDEX_NODE:
        node = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        ast = member_index_node_new(node, node1, node->loc);
        break;
    case FUNC_TYPE_NODE:
        assert(rule->action.item_index_count == 3);
        node = items[rule->action.item_index[0]].ast; //fun name
        assert(node->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[2]].ast; // parameters
        assert(node1->node_type == BLOCK_NODE);
        if (array_size(&node1->block->nodes)) {
            node2 = *(struct ast_node **)array_back(&node1->block->nodes);
            if (node2->node_type > TOTAL_NODE && (node2->node_type >> 16 == TOKEN_VARIADIC)) {
                is_variadic = true;
                array_pop(&node1->block->nodes);
            }
        }
        node3 = items[rule->action.item_index[1]].ast; //return type name
        type_name = node3->node_type == UNIT_NODE ? node3->annotated_type_name : node3->ident->name;
        ast = func_type_node_default_new(node->ident->name, node1, type_name, is_variadic, true, node->loc);
        break;
    case FUNC_NODE:
        node = items[rule->action.item_index[0]].ast; //fun name
        assert(node->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast; //parameters
        assert(node1->node_type == BLOCK_NODE);
        if (array_size(&node1->block->nodes)) {
            node2 = *(struct ast_node **)array_back(&node1->block->nodes);
            if (node2->node_type > TOTAL_NODE && (node2->node_type >> 16 == TOKEN_VARIADIC)) {
                is_variadic = true;
                array_pop(&node1->block->nodes);
            }
        }
        node3 = items[rule->action.item_index[2]].ast;
        if (node3->node_type != BLOCK_NODE) {
            // convert to block node even it's a one line statement
            node3 = _wrap_as_block_node(node3);
        }
        if (rule->action.item_index_count == 4){
            //has return type
            node4 = items[rule->action.item_index[3]].ast;
        }
        if(node4){
            type_name = node4->node_type == UNIT_NODE ? node4->annotated_type_name : node4->ident->name;
        }
        struct ast_node *ft = func_type_node_default_new(node->ident->name, node1, type_name, is_variadic, false, node->loc);
        ast = function_node_new(ft, node3, node->loc);
        hashtable_set_int(symbol_2_int_types, ft->ft->name, TYPE_FUNCTION);
        break;
    case CALL_NODE:
        assert(rule->action.item_index_count == 2);
        node = items[rule->action.item_index[0]].ast;
        assert(node->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        assert(node1->node_type == BLOCK_NODE);
        ast = call_node_new(node->ident->name, node1, node->loc);
        break;
    case UNION_NODE:
    case ENUM_NODE:
    case STRUCT_NODE: // new type definition, like struct in C
        assert(rule->action.item_index_count == 2);
        node = items[rule->action.item_index[0]].ast;
        assert(node->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        if(node1->node_type != BLOCK_NODE){
            node1 = _wrap_as_block_node(node1);
        }
        ast = struct_node_new(node->ident->name, node1, node->loc);
        hashtable_set_int(symbol_2_int_types, node->ident->name, TYPE_STRUCT);
        break;
    case STRUCT_INIT_NODE: 
        assert(rule->action.item_index_count == 2);
        node = items[rule->action.item_index[0]].ast;
        assert(node->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        assert(node1->node_type == BLOCK_NODE);
        ast = struct_init_node_new(node1, node->ident->name, node->loc);
        break;
    case LIST_COMP_NODE:
        node = items[rule->action.item_index[0]].ast;
        ast = list_comp_node_new(node, node->loc);
        break;
    case BLOCK_NODE:
        if (rule->action.item_index_count == 0){
            struct array nodes;
            array_init(&nodes, sizeof(struct ast_node *));
            ast = block_node_new(&nodes);
        }
        else if (rule->action.item_index_count == 1){
            ast = _wrap_as_block_node(items[rule->action.item_index[0]].ast);
        }
        else if(rule->action.item_index_count == 2){
            ast = items[rule->action.item_index[0]].ast;
            if(ast->node_type != BLOCK_NODE){
                ast = _wrap_as_block_node(ast);
            }
            node = items[rule->action.item_index[1]].ast;
            if(node->node_type){
                block_node_add(ast, node);
            }
        }
        break;
    }
    return ast;
}

struct ast_node *parse_code(struct parser *parser, const char *code)
{
    struct ast_node *ast = 0;
    _push_state(parser, 0, 0); 
    struct lexer *lexer = lexer_new_with_string(code);
    if(!lexer) return 0;
    struct token *tok = get_tok(lexer);
    u8 ti = get_token_index(tok->token_type, tok->opcode);
    u16 si, tsi;
    struct parse_rule *rule;
    struct stack_item *s_item;
    struct parser_action *pa;
    //driver 
    while(1){
        si = _get_top_state(parser)->state_index;
        pa = &(*parser->pt)[si][ti];
        if(pa->code == S){
            ast = _build_terminal_ast(tok);
            _push_state(parser, pa->state_index, ast);
            tok = get_tok(lexer);
            ti = get_token_index(tok->token_type, tok->opcode);
        }else if(pa->code == R){
            //do reduce action and build ast node
            rule = &(*parser->pr)[pa->rule_index];
            s_item = _get_start_item(parser, rule->symbol_count);
            ast = _build_nonterm_ast(&parser->symbol_2_int_types, rule, s_item); //build ast according to the rule 
            _pop_states(parser, rule->symbol_count);
            tsi = _get_top_state(parser)->state_index;
            assert((*parser->pt)[tsi][rule->lhs].code == G);
            _push_state(parser, (*parser->pt)[tsi][rule->lhs].state_index, ast);
            //
        }else if(pa->code == A){
            s_item = _pop_state(parser);
            ast = s_item->ast;
            break;
        }else if(tok->token_type == TOKEN_ERROR){
            struct error_report *er = get_last_error_report(lexer);
            printf("%s location (line, col): (%d, %d)\n", er->error_msg, er->loc.line, er->loc.col);
            ast = 0;
            break;
        }else{
            //error recovery
            char *got_symbol = string_get(get_symbol_by_index(ti));
            struct parse_state_items *psi = &(*parser->pstd)[si];
            for(u32 i = 0; i < psi->item_count; i++){
                rule = &(*parser->pr)[psi->items[i].rule];
                u8 parsed = psi->items[i].dot;
                if(parsed == rule->symbol_count){
                    //rule is complete, but 
                    printf("symbol [%s] is not expected after grammar [%s].\n", got_symbol, rule->rule_string);
                }else{
                    char *next_symbol = string_get(get_symbol_by_index(rule->rhs[parsed]));
                    if(!is_terminal(rule->rhs[parsed])){
                        //printf("skip next symbol: %s\n", next_symbol);
                        continue;
                    }
                    printf("symbol [%s] is expected to parse [%s] but got [%s].\n", next_symbol, psi->items[i].item_string, got_symbol);
                }
            }
            // printf("the parser stack is: \n");
            // for (i = 0; i < parser->stack_top; i++){
            //     printf("%d,", parser->stack[i].state_index);
            // }
            // printf("\n");
            ast = 0;
            break;
        }
    }
    lexer_free(lexer);
    return ast;
}

struct ast_node *parse_repl_code(struct parser *parser, void (*fun)(void *, struct ast_node *), void *jit)
{
    return parse_file(parser, 0);
}

struct ast_node *parse_file(struct parser *parser, const char *file_name)
{
    const char *code = read_text_file(file_name);
    struct ast_node * block = parse_code(parser, code);
    free((void*)code);
    return block;
}
