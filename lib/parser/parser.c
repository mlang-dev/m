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
#include "clib/stack.h"
#include "clib/util.h"
#include "app/error.h"
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
    parser->tc = type_context_new();
    return parser;
}

struct parser *parser_new(void)
{
    return _parser_new(&lang_parsing_table, &lang_parsing_rules, &lang_parsing_symbols, &lang_parsing_states);
}

void parser_free(struct parser *parser)
{
    type_context_free(parser->tc);
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
    if(parser->stack_top == 0) return 0;
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

struct ast_node *_build_terminal_ast(struct type_context *tc, struct token *tok)
{
    struct ast_node *ast = 0;
    switch(tok->token_type){
        default:
            ast = token_node_new(tok->token_type, tok->opcode, tok->loc);
            break;
        case TOKEN_EOF:
            ast = ast_node_new(NULL_NODE, tok->loc);
            break;
        case TOKEN_OP:
            ast = token_node_new(tok->token_type, tok->opcode, tok->loc);
            break;
        case TOKEN_IDENT:
            ast = ident_node_new(tok->symbol_val, tok->loc);
            break;
        case TOKEN_LITERAL_INT:
            ast = int_node_new(tc, tok->int_val, tok->loc);
            break;
        case TOKEN_LITERAL_FLOAT:
            ast = double_node_new(tc, tok->double_val,tok->loc);
            break;
        case TOKEN_TRUE:
            ast = bool_node_new(tc, true, tok->loc);
            break;
        case TOKEN_FALSE:
            ast = bool_node_new(tc, false, tok->loc);
            break;
        case TOKEN_LITERAL_CHAR:
            ast = char_node_new(tc, tok->int_val, tok->loc);
            break;
        case TOKEN_LITERAL_STRING:
            ast = string_node_new(tc, tok->str_val, tok->loc);
            tok->str_val = 0; //ownership moved 
            break;
        }
    return ast;
}

enum ast_action {
    MarkMutVar = 0,
    MarkMutType = 1,
    MarkHeapAlloc = 2,
};

struct ast_node *_do_action(enum ast_action action, u8 param, struct ast_node *node)
{
    switch (action)
    {
    case MarkMutVar:
        /* code */
        if(node->node_type == VAR_NODE){
            node->var->mut = param;
        }else if(node->node_type == BLOCK_NODE){
            for(u32 i = 0; i < array_size(&node->block->nodes); i++){
                struct ast_node *elm = array_get_ptr(&node->block->nodes, i);
                assert(elm->node_type == VAR_NODE);
                elm->var->mut = (enum Mut)param;
            }
        }
        break;
    case MarkMutType:
        assert(node->node_type == TYPE_ITEM_NODE);
        node->type_item_node->mut = Mutable;
        break;
    case MarkHeapAlloc:
        node->is_heap_alloc = param;
        break;
    }
    return node;
}

struct ast_node *_take(struct ast_node *nodes[], u8 index)
{
    struct ast_node *node = nodes[index];
    nodes[index] = 0;
    return node;
}

struct ast_node *_build_nonterm_ast(struct type_context *tc, struct parse_rule *rule, struct stack_item *items)
{
    struct ast_node *ast = 0;
    bool is_variadic = false;
    struct ast_node *nodes[rule->symbol_count];
    for(u8 i=0;i<rule->symbol_count; i++){
        nodes[i] = items[i].ast; //intialize all asts on the stack
    }
    if (!rule->action.node_type){
        if (rule->action.item_index_count == 0){
            ast = _take(nodes, 0);
        }else if(rule->action.item_index_count == 3){
            //this is special for action
            ast = _take(nodes, rule->action.item_index[2]);
            ast = _do_action(rule->action.item_index[0], rule->action.item_index[1], ast);
        }else{
            ast = _take(nodes, rule->action.item_index[0]);
        }
    }else{
        enum IndexType index_type;
        switch (rule->action.node_type) {
        case NULL_NODE:
        case TOTAL_NODE:
        case LITERAL_NODE:
        case IDENT_NODE:
        case TOKEN_NODE:
            printf("type: %d is not supported for nonterm node.", rule->action.node_type);
            break;
        case IMPORT_NODE:
        {
            struct ast_node *module = _take(nodes, rule->action.item_index[0]);
            struct ast_node *object = _take(nodes, rule->action.item_index[1]);
            ast = import_node_new(module->ident->name, object, module->loc);
            //we need to free module
            node_free(module);
            break;
        }
        case MEMORY_NODE:
        {
            struct ast_node *initial = _take(nodes, rule->action.item_index[0]);
            struct ast_node *max = 0;
            if (rule->action.item_index_count==2){
                max = _take(nodes, rule->action.item_index[1]);
            }
            ast = memory_node_new(initial, max, initial->loc);
            break;
        }
        case UNARY_NODE:
        {        
            assert(rule->action.item_index_count==2);
            struct ast_node *op = _take(nodes, rule->action.item_index[0]);
            assert(op->node_type == TOKEN_NODE);
            enum op_code opcode = op->token->token_op;
            struct ast_node *operand = _take(nodes, rule->action.item_index[1]);
            ast = unary_node_new(opcode, operand, rule->action.item_index[0] > rule->action.item_index[1], op->loc);
            node_free(op); 
            break;
        }
        case CAST_NODE:
        {
            assert(rule->action.item_index_count==2);
            struct ast_node *to_type_item_node = _take(nodes, rule->action.item_index[0]);
            struct ast_node *expr = _take(nodes, rule->action.item_index[1]);
            ast = cast_node_new(to_type_item_node, expr, to_type_item_node->loc);
            break;
        }
        case TYPE_EXPR_ITEM_NODE:
        {
            u8 item_kind = rule->action.item_index[0];
            struct ast_node *ident = 0;
            struct ast_node *is_of_type = 0;
            if(item_kind){ //named type
                ident = _take(nodes, rule->action.item_index[1]);
                is_of_type = _take(nodes, rule->action.item_index[2]);
            }else{//type only
                is_of_type = _take(nodes, rule->action.item_index[1]);
            }
            ast = type_expr_item_node_new(ident, is_of_type, ident ? ident->loc:is_of_type->loc);
            break;
        }
        case VAR_NODE:
        {
            struct ast_node *node = _take(nodes, rule->action.item_index[1]);
            struct ast_node *var = 0;
            struct ast_node *type_item_node = 0;
            struct ast_node *init_value = 0;
            u8 var_kind = rule->action.item_index[0];
            //0: ident
            //1: ident:type
            //2: ident = init or ident:type = init
            switch (var_kind){
            case 0:
                //id
                var = node;
                break;
            case 1:
                //id:type
                assert(node->node_type == TYPE_EXPR_ITEM_NODE);
                var = node->type_expr_item->ident;
                type_item_node = node->type_expr_item->is_of_type;
                ast_node_free(node);
                break;
            case 2:
                //id = init value
                if(node->node_type == VAR_NODE){
                    ast = node;
                    ast->var->init_value = _take(nodes, rule->action.item_index[2]);
                } else if(node->node_type == BLOCK_NODE){
                    //tuple unpack
                    ast = node;
                    init_value = _take(nodes, rule->action.item_index[2]);
                    struct ast_node *temp_var_node = 0;
                    if(init_value->node_type != IDENT_NODE){
                        symbol temp_var = get_temp_symbol();
                        temp_var_node = var_node_new(ident_node_new(temp_var, init_value->loc), 0, init_value, false, Immutable, init_value->loc);
                        init_value = ident_node_new(temp_var, init_value->loc);
                    }
                    for(size_t i = 0; i < array_size(&node->block->nodes); i++){
                        struct ast_node *n = array_get_ptr(&node->block->nodes, i);
                        assert(n->node_type == VAR_NODE);
                        n->var->init_value = member_index_node_new(IndexTypeInteger, init_value, int_node_new(tc, i, init_value->loc), init_value->loc);
                        if(i < array_size(&node->block->nodes) - 1){
                            init_value = node_copy(tc, init_value);
                        }
                    }
                    if(temp_var_node){
                        array_insert_at(&node->block->nodes, &temp_var_node, 0);
                    }
                } else {
                    assert(false);
                }
                break;
            default:
                break;
            }
            if(ast==0){
                assert(!type_item_node || type_item_node->node_type == TYPE_ITEM_NODE);
                ast = var_node_new(var, type_item_node, init_value, false, false, var->loc);
            }
            break;
        }
        case RANGE_NODE:
        {
            struct ast_node *start = _take(nodes, rule->action.item_index[0]); //start
            struct ast_node *step = 0;
            struct ast_node *end;
            if(rule->action.item_index_count == 3){
                step = _take(nodes, rule->action.item_index[1]); //step
                end = _take(nodes, rule->action.item_index[2]); // end
            } else {
                end = _take(nodes, rule->action.item_index[1]); // end
            }
            ast = range_node_new(start, end, step, start->loc);
            break;
        }
        case FOR_NODE:
        {
            struct ast_node *var = _take(nodes, rule->action.item_index[0]);
            assert(var->node_type == VAR_NODE);
            struct ast_node *range = _take(nodes, rule->action.item_index[1]);
            struct ast_node *body = _take(nodes, rule->action.item_index[2]); 
            ast = for_node_new(var, range, body, var->loc);
            break;
        }
        case WHILE_NODE:
        {
            struct ast_node *expr = _take(nodes, rule->action.item_index[0]); //expr
            struct ast_node * body = _take(nodes, rule->action.item_index[1]); //body
            ast = while_node_new(expr, body, expr->loc);
            break;
        }
        case JUMP_NODE:
        {
            struct ast_node *token_node = _take(nodes, rule->action.item_index[0]); //token node
            assert(token_node->node_type == TOKEN_NODE);
            enum token_type token_type = token_node->token->token_type;
            struct ast_node *expr = 0;
            if (rule->action.item_index_count == 2)
                expr = _take(nodes, rule->action.item_index[1]); //expr
            ast = jump_node_new(token_type, expr, token_node->loc);
            node_free(token_node);
            break;
        }
        case IF_NODE:
        {
            struct ast_node *cond = _take(nodes, rule->action.item_index[0]);
            struct ast_node *then_expr = _take(nodes, rule->action.item_index[1]);
            struct ast_node *else_expr = 0;
            if (rule->action.item_index_count == 3){
                else_expr = _take(nodes, rule->action.item_index[2]);
            }
            ast = if_node_new(cond, then_expr, else_expr, cond->loc);
            break;
        }
        case MATCH_NODE:
        {
            struct ast_node *test_expr = _take(nodes, rule->action.item_index[0]);
            struct ast_node *match_cases = _take(nodes, rule->action.item_index[1]);
            ast = match_node_new(test_expr, match_cases, test_expr->loc);
            break;
        }
        case MATCH_CASE_NODE:
        {
            struct ast_node *pattern = _take(nodes, rule->action.item_index[0]);
            struct ast_node *cond_expr = 0;
            struct ast_node *expr;
            if(rule->action.item_index_count == 3){
                cond_expr = _take(nodes, rule->action.item_index[1]);
                expr = _take(nodes, rule->action.item_index[2]);
            } else {
                expr = _take(nodes, rule->action.item_index[1]);
            }
            ast = match_item_node_new(pattern, cond_expr, expr, pattern->loc);
            break;
        }
        case WILDCARD_NODE:
        {
            struct ast_node *wildcard_node = _take(nodes, 0);
            ast = ast_node_new(WILDCARD_NODE, wildcard_node->loc);
            node_free(wildcard_node);
            break;
        }
        case BINARY_NODE:
        {
            struct ast_node *op = _take(nodes, rule->action.item_index[1]);
            assert(op->node_type == TOKEN_NODE);
            enum op_code opcode = op->token->token_op;
            struct ast_node *lhs = _take(nodes, rule->action.item_index[0]);
            struct ast_node *rhs = _take(nodes, rule->action.item_index[2]);
            ast = binary_node_new(opcode, lhs, rhs, lhs->loc);
            node_free(op);
            break;
        }
        case ASSIGN_NODE:
        {
            struct ast_node *op = _take(nodes, rule->action.item_index[1]);
            assert(op->node_type == TOKEN_NODE);
            enum op_code opcode = op->token->token_op;
            struct ast_node *lhs = _take(nodes, rule->action.item_index[0]);
            struct ast_node *rhs = _take(nodes, rule->action.item_index[2]);
            ast = assign_node_new(opcode, lhs, rhs, lhs->loc);
            node_free(op);
            break;
        }
        case MEMBER_INDEX_NODE:
        {
            index_type = rule->action.item_index[0];
            struct ast_node *object = _take(nodes, rule->action.item_index[1]);
            struct ast_node *index = _take(nodes, rule->action.item_index[2]);
            ast = member_index_node_new(index_type, object, index, object->loc);
            break;
        }
        case FUNC_TYPE_NODE:
        {
            assert(rule->action.item_index_count == 3);
            struct ast_node *ft_name = _take(nodes, rule->action.item_index[0]); //func name
            assert(ft_name->node_type == IDENT_NODE);
            struct ast_node *parameters = _take(nodes, rule->action.item_index[2]); // parameters
            assert(parameters->node_type == BLOCK_NODE);
            if (array_size(&parameters->block->nodes)) {
                struct ast_node *last_param = array_back_ptr(&parameters->block->nodes);
                if (last_param->node_type == TOKEN_NODE && (last_param->token->token_type == TOKEN_VARIADIC)) {
                    is_variadic = true;
                    node_free(array_pop_p(&parameters->block->nodes));
                }
            }
            struct ast_node *ret_type_name = _take(nodes, rule->action.item_index[1]); //return type name
            ast = func_type_item_node_default_new(tc, ft_name->ident->name, parameters, 0, ret_type_name, is_variadic, true, ft_name->loc);
            node_free(ft_name);
            break;
        }
        case FUNC_NODE:
        {
            struct ast_node *func_name = _take(nodes, rule->action.item_index[0]); //func name
            assert(func_name->node_type == IDENT_NODE);
            struct ast_node *parameters = _take(nodes, rule->action.item_index[1]); //parameters
            assert(parameters->node_type == BLOCK_NODE);
            if (array_size(&parameters->block->nodes)) {
                struct ast_node *last_param = array_back_ptr(&parameters->block->nodes);
                if (last_param->node_type == TOKEN_NODE && (last_param->token->token_type == TOKEN_VARIADIC)) {
                    is_variadic = true;
                    node_free(array_pop_p(&parameters->block->nodes));
                }
            }
            struct ast_node *func_body = _take(nodes, rule->action.item_index[2]);
            if (func_body->node_type != BLOCK_NODE) {
                // convert to block node even it's a one line statement
                func_body = wrap_as_block_node(func_body);
            }
            struct ast_node *ret_type_item_node = 0;
            if (rule->action.item_index_count == 4){
                //has return type
                ret_type_item_node = _take(nodes, rule->action.item_index[3]);
            }
            struct ast_node *ft = func_type_item_node_default_new(tc, func_name->ident->name, parameters, 0, ret_type_item_node, is_variadic, false, func_name->loc);
            ast = function_node_new(ft, func_body, func_name->loc);
            hashtable_set_int(&tc->symbol_2_int_types, ft->ft->name, TYPE_FUNCTION);
            node_free(func_name);
            break;
        }
        case CALL_NODE:
        {
            assert(rule->action.item_index_count == 2);
            struct ast_node *callee_name = _take(nodes, rule->action.item_index[0]);
            assert(callee_name->node_type == IDENT_NODE);
            struct ast_node *arguments = _take(nodes, rule->action.item_index[1]);
            assert(arguments->node_type == BLOCK_NODE);
            ast = call_node_new(callee_name->ident->name, arguments, callee_name->loc);
            node_free(callee_name);
            break;
        }
        case VARIANT_TYPE_ITEM_NODE:
        {
            enum UnionKind kind = rule->action.item_index[0];
            struct ast_node *tag_id = _take(nodes, rule->action.item_index[1]);
            struct ast_node *tag_value = 0;
            switch(kind){
                case UntaggedUnion:
                {
                    //id:type
                    struct ast_node *node = tag_id;
                    tag_id = node->type_expr_item->ident;
                    tag_value = node->type_expr_item->is_of_type;
                    ast_node_free(node);
                    break;
                }
                case TaggedUnion:
                case EnumTagValue:
                    tag_value = _take(nodes, rule->action.item_index[2]);
                    break;
                case EnumTagOnly:
                    break;
            }
            symbol tag = tag_id->ident->name;
            ast = variant_type_node_new(kind, tag, tag_value, tag_id->loc);
            node_free(tag_id);
            break;
        }
        case VARIANT_NODE:
        case STRUCT_NODE: // new type definition, like struct in C
        {
            assert(rule->action.item_index_count == 2);
            struct ast_node *struct_name = _take(nodes, rule->action.item_index[0]);
            assert(struct_name->node_type == IDENT_NODE);
            struct ast_node *struct_body = _take(nodes, rule->action.item_index[1]);
            if(struct_body->node_type != BLOCK_NODE){
                struct_body = wrap_as_block_node(struct_body);
            }
            ast = adt_node_new(rule->action.node_type, struct_name->ident->name, struct_body, struct_name->loc);
            hashtable_set_int(&tc->symbol_2_int_types, struct_name->ident->name, TYPE_STRUCT);
            node_free(struct_name);
            break;
        }
        case ADT_INIT_NODE: 
        {
            struct ast_node *adt_name = 0;
            struct ast_node *init_body;
            enum ADTInitKind kind = rule->action.item_index[0];
            if(rule->action.item_index_count == 3){
                adt_name = _take(nodes, rule->action.item_index[1]);
                init_body = _take(nodes, rule->action.item_index[2]);
            } else if (rule->action.item_index_count == 2){
                init_body = _take(nodes, rule->action.item_index[1]);
            } else {
                assert(false);
            }
            struct ast_node *type_item_node = 0; 
            if(adt_name) type_item_node = type_item_node_new_with_type_name(adt_name->ident->name, Immutable, adt_name->loc);
            ast = adt_init_node_new(kind, init_body, type_item_node, adt_name ? adt_name->loc : init_body->loc);
            if(adt_name) node_free(adt_name);
            break;
        }
        case ARRAY_INIT_NODE:
        {
            struct ast_node *init_expr = 0; //array initialization expression in the braket
            if(rule->action.item_index_count){
                init_expr = _take(nodes, rule->action.item_index[0]);
            }
            ast = array_init_node_new(init_expr, items[0].ast->loc);
            break;
        }
        case ARRAY_TYPE_NODE:
        {
            struct ast_node *elm_type_name = _take(nodes, rule->action.item_index[0]);
            struct ast_node *dims = _take(nodes, rule->action.item_index[1]);
            ast = array_type_node_new(elm_type_name, dims, elm_type_name->loc);
            break;
        }
        case TYPE_ITEM_NODE:
        {
            enum TypeNodeKind type_item_node_kind = rule->action.item_index[0];
            struct ast_node *node = _take(nodes, rule->action.item_index[1]);
            switch(type_item_node_kind){
            case BuiltinType:
            {
                assert(node->node_type == TOKEN_NODE);
                struct token_pattern *tp = get_token_pattern_by_token_type(node->token->token_type);
                ast = type_item_node_new_with_builtin_type(tp->symbol_name, Immutable, node->loc);
                break;
            }
            case TypeName:
                ast = type_item_node_new_with_type_name(node->ident->name, Immutable, node->loc);
                break;
            case ArrayType:
                ast = type_item_node_new_with_array_type(node->array_type, Immutable, node->loc);
                node->array_type = 0; //to prevent its being freed
                break;
            case TupleType:
                ast = type_item_node_new_with_tuple_type(node, Immutable, node->loc);
                node = 0;//not to be freed
                break;
            case RefType:
                assert(node->node_type == TYPE_ITEM_NODE);
                ast = type_item_node_new_with_ref_type(node->type_item_node, Immutable, node->loc);
                node->type_item_node = 0;
                break;
            }
            node_free(node);
            break;
        }
        case TYPE_NODE:
        {
            struct ast_node *ident = nodes[rule->action.item_index[0]];
            struct ast_node *node = _take(nodes, rule->action.item_index[1]);
            ast = type_node_new(ident->ident->name, node, ident->loc);
            break;
        }
        case NEW_NODE:
        {
            struct ast_node *node = _take(nodes, rule->action.item_index[0]);
            ast = new_node_new(node, node->loc);
            break;
        }
        case DEL_NODE:
        {
            struct ast_node *ident = _take(nodes, rule->action.item_index[0]);
            ast = del_node_new(ident, ident->loc);
            break;
        }
        case BLOCK_NODE:
            if (rule->action.item_index_count == 0){
                struct array nodes;
                array_init(&nodes, sizeof(struct ast_node *));
                ast = block_node_new(&nodes);
            }
            else if (rule->action.item_index_count == 1){
                ast = _take(nodes, rule->action.item_index[0]);
                if(ast->node_type != BLOCK_NODE)
                    ast = wrap_as_block_node(ast);
            }
            else if(rule->action.item_index_count == 2){
                ast = _take(nodes, rule->action.item_index[0]);
                if(ast->node_type != BLOCK_NODE){
                    ast = wrap_as_block_node(ast);
                }
                struct ast_node *node = _take(nodes, rule->action.item_index[1]);
                if(node->node_type){
                    block_node_add(ast, node);
                    if(node->node_type == BLOCK_NODE){
                        free_block_node(node, false); //we shallow release the shell block node
                    }
                }
            }
            break;
        }
    }
    //free unused nodes (with pointer value is not zero)
    for(u8 i = 0; i < rule->symbol_count; i++){
        if(nodes[i]){
            //not used nodes
            node_free(nodes[i]);
        }
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
            ast = _build_terminal_ast(parser->tc, tok);
            _push_state(parser, pa->state_index, ast);
            tok = get_tok(lexer);
            ti = get_terminal_token_index(tok->token_type, tok->opcode);
        }else if(pa->code == R){
            //do reduce action and build ast node
            rule = &(*parser->pr)[pa->rule_index];
            s_item = _get_start_item(parser, rule->symbol_count);
            ast = _build_nonterm_ast(parser->tc, rule, s_item); //build ast according to the rule 
            _pop_states(parser, rule->symbol_count);
            tsi = _get_top_state(parser)->state_index;
            assert((*parser->pt)[tsi][rule->lhs].code == G);
            _push_state(parser, (*parser->pt)[tsi][rule->lhs].state_index, ast);
            //
        }else if(pa->code == A){
            s_item = _pop_state(parser);
            ast = s_item->ast;
            break;
        }else if(tok->token_type == TOKEN_NULL){
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
                    printf("symbol %s is not expected after grammar rule %s\n", got_symbol, rule->rule_string);
                }else{
                    const char *next_symbol = (*parser->psd)[rule->rhs[parsed]];
                    printf("symbol %s is expected to parse %s but got %s\n", next_symbol, psi->items[i].item_string, got_symbol);
                }
            }
            //clean stack items
            while((s_item = _pop_state(parser))){
                node_free(s_item->ast);
            }
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

struct ast_node *parse_file(struct parser *parser, const char *file_path)
{
    const char *code = read_text_file(file_path);
    struct ast_node * block = parse_code(parser, code);
    free((void*)code);
    return block;
}
