/*
 * lalr_parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement an LALR parser main driver, taking a parsing table & rule and parse text 
 * into ast according to the parsing table and rule set
 */
#include "parser/lalr_parser.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include <assert.h>


struct lalr_parser *lalr_parser_new(const char *grammar_text)
{
    struct lalr_parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->stack_top = 0;
    parser->pg = lalr_parser_generator_new(grammar_text);
    return parser;
}

void lalr_parser_free(struct lalr_parser *parser)
{
    lalr_parser_generator_free(parser->pg);
    FREE(parser);
}

void _push_state(struct lalr_parser *parser, u16 state, struct ast_node *ast)
{
    struct stack_item *si = &parser->stack[parser->stack_top++];
    si->state_index = state;
    si->ast = ast;
}

struct stack_item *_pop_state(struct lalr_parser *parser)
{
    return &parser->stack[--parser->stack_top];
}

struct stack_item *_get_top_state(struct lalr_parser *parser)
{
    return &parser->stack[parser->stack_top-1];
}

struct stack_item *_get_start_item(struct lalr_parser *parser, u8 symbol_count)
{
    return &parser->stack[parser->stack_top-symbol_count];
}

void _pop_states(struct lalr_parser *parser, u8 symbol_count)
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
            node_type = token_to_node_type(tok->token_type, tok->opcode);
            ast = ast_node_new(node_type, 0, tok->loc);
            break;
        case TOKEN_IDENT:
            ast = ident_node_new(tok->symbol_val, tok->loc);
            break;
        case TOKEN_INT:
            ast = int_node_new(tok->int_val, tok->loc);
            break;
        case TOKEN_FLOAT:
            ast = double_node_new(tok->double_val,tok->loc);
            break;
        }
    return ast;
}

struct ast_node *_build_nonterm_ast(struct parse_rule *rule, struct stack_item *items)
{
    enum op_code opcode;
    struct ast_node *ast = 0;
    struct ast_node *node = 0;
    struct ast_node *rhs = 0;
    if (!rule->action.action){
        if (rule->action.exp_item_index_count == 0)
            return items[0].ast;
        else{
            return items[rule->action.exp_item_index[0]].ast;
        }
    }
    enum node_type node_type = symbol_to_node_type(rule->action.action);
    switch (node_type) {
    default:
        assert(false);
        break;
    case UNARY_NODE:
        node = items[0].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[1].ast;
        ast = unary_node_new(opcode, node, node->loc);
        break;
    case BINARY_NODE:
        node = items[1].ast;
        opcode = node->node_type & 0xFFFF;
        node = items[0].ast;
        rhs = items[2].ast;
        ast = binary_node_new(opcode, node, rhs, node->loc);
        break;
    case FUNC_NODE:
        node = items[0].ast;
        assert(node->node_type == IDENT_NODE);
        ARRAY_FUN_PARAM(fun_params);
        struct ast_node *ft = func_type_node_default_new(node->ident->name, &fun_params, 0, false, false, node->loc);
        node = items[1].ast;
        ast = function_node_new(ft, node, node->loc);
        break;
    }
    return ast;
}

struct ast_node *parse_text(struct lalr_parser *parser, const char *text)
{
    struct ast_node *ast = 0;
    _push_state(parser, 0, 0); 
    struct lexer *lexer = lexer_new_for_string(text);
    struct token *tok = get_tok(lexer);
    u8 a = get_token_index(tok->token_type, tok->opcode);
    u16 s, t;
    struct parse_rule *rule;
    struct stack_item *si;
    struct parser_action *pa;
    //driver 
    while(1){
        s = _get_top_state(parser)->state_index;
        pa = &parser->pg->parsing_table[s][a];
        if(pa->code == ACTION_SHIFT){
            ast = _build_terminal_ast(tok);
            _push_state(parser, pa->state_index, ast);
            tok = get_tok(lexer);
            a = get_token_index(tok->token_type, tok->opcode);
        }else if(pa->code == ACTION_REDUCE){
            //do reduce action and build ast node
            rule = &parser->pg->rules[pa->rule_index];
            si = _get_start_item(parser, rule->symbol_count);
            ast = _build_nonterm_ast(rule, si); //build ast according to the rule 
            _pop_states(parser, rule->symbol_count);
            t = _get_top_state(parser)->state_index;
            assert(parser->pg->parsing_table[t][rule->lhs].code == ACTION_GOTO);
            _push_state(parser, parser->pg->parsing_table[t][rule->lhs].state_index, ast);
            //
        }else if(pa->code == ACTION_ACCEPT){
            si = _pop_state(parser);
            ast = si->ast;
            break;
        }else{
            //error recovery
            break;
        }
    }
    lexer_free(lexer);
    return ast;
}
