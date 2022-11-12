/*
 * parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a parser's main driver, taking a parsing table & rule and parse text 
 * into ast according to the parsing table and rule set
 */
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/m_parsing_table.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include "error/error.h"
#include "parser/ast.h"
#include "sema/type.h"
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

#define TOKEN_TO_NODE_TYPE(token_type)  (token_type << 16)
#define NODE_TO_TOKEN_TYPE(node_type)   (node_type >> 16)

struct ast_node *_build_terminal_ast(struct token *tok)
{
    enum node_type node_type;
    struct ast_node *ast = 0;
    switch(tok->token_type){
        default:
            node_type = tok->token_type << 16;
            ast = ast_node_new(node_type, tok->loc);
            break;
        case TOKEN_EOF:
            ast = ast_node_new(NULL_NODE, tok->loc);
            break;
        case TOKEN_UNIT:
            ast = ast_node_new(UNIT_NODE, tok->loc);
            break;
        case TOKEN_OP:
            //*hacky way to transfer opcode
            node_type = (tok->token_type << 16) | tok->opcode;
            ast = ast_node_new(node_type, tok->loc);
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
    struct ast_node *node1 = 0;
    struct ast_node *node2 = 0;
    struct ast_node *node3 = 0;
    struct ast_node *node4 = 0;
    bool is_variadic = false;
    if (!rule->action.node_type){
        if (rule->action.item_index_count == 0){
            return items[0].ast;
        }else{
            return items[rule->action.item_index[0]].ast;
        }
    }
    enum aggregate_type aggregate_type;
    switch (rule->action.node_type) {
    case NULL_NODE:
    case TOTAL_NODE:
    case UNIT_NODE:
    case LITERAL_NODE:
    case IDENT_NODE:
        printf("type: %d is not supported for nonterm node.", rule->action.node_type);
        break;
    case IMPORT_NODE:
    {
        struct ast_node *module = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        ast = import_node_new(module->ident->name, node1, module->loc);
        break;
    }
    case MEMORY_NODE:
    {
        struct ast_node *initial = items[rule->action.item_index[0]].ast;
        if (rule->action.item_index_count==2){
            node1 = items[rule->action.item_index[1]].ast;
        }
        ast = memory_node_new(initial, node1, initial->loc);
        break;
    }
    case UNARY_NODE:
    {        
        assert(rule->action.item_index_count==2);
        struct ast_node *op = items[rule->action.item_index[0]].ast;
        opcode = op->node_type & 0xFFFF;
        struct ast_node *operand = items[rule->action.item_index[1]].ast;
        ast = unary_node_new(opcode, operand, rule->action.item_index[0] > rule->action.item_index[1], op->loc);
        break;
    }
    case CAST_NODE:
    {
        assert(rule->action.item_index_count==2);
        struct ast_node *to_type_node = items[rule->action.item_index[0]].ast;
        struct ast_node *expr = items[rule->action.item_index[1]].ast;
        ast = cast_node_new(to_type_node, expr, to_type_node->loc);
        break;
    }
    case VAR_NODE:
    {
        struct ast_node *var = items[rule->action.item_index[1]].ast;
        assert(var->node_type == IDENT_NODE);
        if (rule->action.item_index[0]) {
            node1 = items[rule->action.item_index[2]].ast;
            if (rule->action.item_index_count == 4) {
                //has type and has init value
                assert(node1->node_type == IDENT_NODE||node1->node_type == UNARY_NODE||node1->node_type == ARRAY_TYPE_NODE);
                node2 = items[rule->action.item_index[3]].ast;
                ast = var_node_new(var->ident->name, node1, node2, false, var->loc);
            } else { // has no type info, has init value
                ast = var_node_new(var->ident->name, 0, node1, false, var->loc);
            }
        } else if (rule->action.item_index_count > 2) {
            //just has ID and type
            node1 = items[rule->action.item_index[2]].ast;
            assert(node1->node_type == IDENT_NODE||node1->node_type == UNARY_NODE||node1->node_type == ARRAY_TYPE_NODE);
            ast = var_node_new(var->ident->name, node1, 0, false, var->loc);
        } else {
            //just ID
            ast = var_node_new(var->ident->name, 0, 0, false, var->loc);
        }
        break;
    }
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
    {
        struct ast_node *var = items[rule->action.item_index[0]].ast;
        assert(var->node_type == VAR_NODE);
        node1 = items[rule->action.item_index[1]].ast; //range
        node2 = items[rule->action.item_index[2]].ast; //body
        node1->range->end = binary_node_new(OP_LT, ident_node_new(var->var->var_name, var->loc), node1->range->end, node1->range->end->loc);
        ast = for_node_new(var, node1, node2, var->loc);
        break;
    }
    case WHILE_NODE:
    {
        struct ast_node *expr = items[rule->action.item_index[0]].ast; //expr
        struct ast_node * body = items[rule->action.item_index[1]].ast; //body
        ast = while_node_new(expr, body, expr->loc);
        break;
    }
    case JUMP_NODE:
    {
        struct ast_node *token_node = items[rule->action.item_index[0]].ast; //token node
        enum token_type token_type = NODE_TO_TOKEN_TYPE(token_node->node_type);
        struct ast_node *expr = 0;
        if (rule->action.item_index_count == 2)
            expr = items[rule->action.item_index[1]].ast; //expr
        ast = jump_node_new(token_type, expr, token_node->loc);
        break;
    }
    case IF_NODE:
    {
        struct ast_node *cond = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        if (rule->action.item_index_count == 3){
            node2 = items[rule->action.item_index[2]].ast;
        }
        ast = if_node_new(cond, node1, node2, cond->loc);
        break;
    }
    case BINARY_NODE:
    {
        struct ast_node *op = items[rule->action.item_index[1]].ast;
        opcode = op->node_type & 0xFFFF;
        struct ast_node *lhs = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[2]].ast;
        ast = binary_node_new(opcode, lhs, node1, lhs->loc);
        break;
    }
    case MEMBER_INDEX_NODE:
    {
        aggregate_type = rule->action.item_index[0];
        struct ast_node *object = items[rule->action.item_index[1]].ast;
        struct ast_node *index = items[rule->action.item_index[2]].ast;
        ast = member_index_node_new(aggregate_type, object, index, object->loc);
        break;
    }
    case FUNC_TYPE_NODE:
    {
        assert(rule->action.item_index_count == 3);
        struct ast_node *ft_name = items[rule->action.item_index[0]].ast; //fun name
        assert(ft_name->node_type == IDENT_NODE);
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
        ast = func_type_node_default_new(ft_name->ident->name, node1, 0, node3, is_variadic, true, ft_name->loc);
        break;
    }
    case FUNC_NODE:
    {
        struct ast_node *func_name = items[rule->action.item_index[0]].ast; //fun name
        assert(func_name->node_type == IDENT_NODE);
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
        struct ast_node *ft = func_type_node_default_new(func_name->ident->name, node1, 0, node4, is_variadic, false, func_name->loc);
        ast = function_node_new(ft, node3, func_name->loc);
        hashtable_set_int(symbol_2_int_types, ft->ft->name, TYPE_FUNCTION);
        break;
    }
    case CALL_NODE:
    {
        assert(rule->action.item_index_count == 2);
        struct ast_node *callee_name = items[rule->action.item_index[0]].ast;
        assert(callee_name->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        assert(node1->node_type == BLOCK_NODE);
        ast = call_node_new(callee_name->ident->name, node1, callee_name->loc);
        break;
    }
    case UNION_NODE:
    case ENUM_NODE:
    case STRUCT_NODE: // new type definition, like struct in C
    {
        assert(rule->action.item_index_count == 2);
        struct ast_node *struct_name = items[rule->action.item_index[0]].ast;
        assert(struct_name->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        if(node1->node_type != BLOCK_NODE){
            node1 = _wrap_as_block_node(node1);
        }
        ast = struct_node_new(struct_name->ident->name, node1, struct_name->loc);
        hashtable_set_int(symbol_2_int_types, struct_name->ident->name, TYPE_STRUCT);
        break;
    }
    case STRUCT_INIT_NODE: 
    {
        assert(rule->action.item_index_count == 2);
        struct ast_node *struct_name = items[rule->action.item_index[0]].ast;
        assert(struct_name->node_type == IDENT_NODE);
        node1 = items[rule->action.item_index[1]].ast;
        assert(node1->node_type == BLOCK_NODE);
        ast = struct_init_node_new(node1, struct_name, struct_name->loc);
        break;
    }
    case ARRAY_INIT_NODE:
    {
        struct ast_node *init_expr = 0; //array initialization expression in the braket
        if(rule->action.item_index_count){
            init_expr = items[rule->action.item_index[0]].ast;
        }
        ast = array_init_node_new(init_expr, items[0].ast->loc);
        break;
    }
    case ARRAY_TYPE_NODE:
    {
        struct ast_node *elm_type_name = items[rule->action.item_index[0]].ast;
        node1 = items[rule->action.item_index[1]].ast;
        ast = array_type_node_new(elm_type_name, node1, elm_type_name->loc);
        break;
    }
    case BLOCK_NODE:
        if (rule->action.item_index_count == 0){
            struct array nodes;
            array_init(&nodes, sizeof(struct ast_node *));
            ast = block_node_new(&nodes);
        }
        else if (rule->action.item_index_count == 1){
            ast = items[rule->action.item_index[0]].ast;
            if(ast->node_type != BLOCK_NODE)
                ast = _wrap_as_block_node(ast);
        }
        else if(rule->action.item_index_count == 2){
            ast = items[rule->action.item_index[0]].ast;
            if(ast->node_type != BLOCK_NODE){
                ast = _wrap_as_block_node(ast);
            }
            struct ast_node *node = items[rule->action.item_index[1]].ast;
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
    u8 ti = get_terminal_token_index(tok->token_type, tok->opcode);
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
            ti = get_terminal_token_index(tok->token_type, tok->opcode);
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
            const char *got_symbol = (*parser->psd)[ti];
            struct parse_state_items *psi = &(*parser->pstd)[si];
            for(u32 i = 0; i < psi->item_count; i++){
                rule = &(*parser->pr)[psi->items[i].rule];
                u8 parsed = psi->items[i].dot;
                if(parsed == rule->symbol_count){
                    //rule is complete, but 
                    printf("symbol [%s] is not expected after grammar [%s].\n", got_symbol, rule->rule_string);
                }else{
                    const char *next_symbol = (*parser->psd)[rule->rhs[parsed]];
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
