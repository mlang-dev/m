/*
 * parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a generic parser, taking a EBNF grammar text and parse text 
 * into ast according to the grammar
 */
#include "parser/parser.h"
#include "clib/array.h"
#include "clib/util.h"
#include "parser/grammar.h"

struct parser *parser_new(const char *grammar_text)
{
    struct grammar *grammar = grammar_parse(grammar_text);
    struct parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->grammar = grammar;
    return parser;
}

void parser_free(struct parser *parser)
{
    grammar_free(parser->grammar);
    free(parser);
}

struct expr_parse *parse_state_find_expr_parse(struct parse_state *state, struct expr* expr, size_t parsed)
{
    for(size_t i = 0; i < array_size(&state->expr_parses); i++){
        struct expr_parse *ep = (struct expr_parse *)array_get(&state->expr_parses, i);
        if(ep->expr == expr && ep->parsed == parsed){
            return ep;
        }
    }
    return 0;
}

void parse_state_init_rule(struct parse_state *state, struct rule *rule)
{
    struct expr_parse ep;
    ep.parsed = 0;
    ep.start_state_index = state->state_index;
    ep.rule = rule;
    for(size_t i = 0; i < array_size(&rule->exprs); i++){
        ep.expr = (struct expr *)array_get(&rule->exprs, i);
        if (!parse_state_find_expr_parse(state, ep.expr, 0)){
            array_push(&state->expr_parses, &ep);
        }
    }
}

void parse_state_init(struct parse_state *state, int state_index)
{
    state->state_index = state_index;
    array_init(&state->expr_parses, sizeof(struct expr_parse));
}

void parse_state_deinit(struct parse_state *state)
{
    array_deinit(&state->expr_parses);
}

void parse_state_advance_expr_parse(struct parse_state *state, struct expr_parse *ep)
{
    if (!parse_state_find_expr_parse(state, ep->expr, ep->parsed + 1)){
        struct expr_parse init_ep;
        init_ep.parsed = ep->parsed + 1;
        init_ep.expr = ep->expr;
        init_ep.rule = ep->rule;
        init_ep.start_state_index = ep->start_state_index;
        array_push(&state->expr_parses, &init_ep);
    }
}

void parse_states_init(struct parse_states *states)
{
    array_init(&states->states, sizeof(struct parse_state));
}

struct parse_state *parse_states_add_state(struct parse_states *states)
{
    struct parse_state state;
    parse_state_init(&state, array_size(&states->states));
    array_push(&states->states, &state);
    return (struct parse_state*)array_back(&states->states);
}

void parse_states_deinit(struct parse_states *states)
{
    for(size_t i = 0; i < array_size(&states->states); i++){
        struct parse_state *state = (struct parse_state *)array_get(&states->states, i);
        parse_state_deinit(state);
    }
    array_deinit(&states->states);
}

bool _is_match(struct tok *tok, struct expr_item *ei)
{
    switch (ei->ei_type){
    default:
        return false;
    case EI_TOKEN_MATCH: // like ID, NUM, STRING, CHAR token
    case EI_EXACT_MATCH: // keyword
        return tok->tok_type == ei->sym;
    case EI_IN_MATCH:
        return expr_item_exists_symbol(ei, tok->tok_type);
    }    
}

void _complete(struct parse_state *state, symbol nonterm, struct parse_state *start_state)
{
    //finding in start_state, all parse expr's next (non-term) symbol is nonterm parameter passed
    //advance one and add to current state
    for (size_t i = 0; i < array_size(&start_state->expr_parses); i++) {
        struct expr_parse *ep = (struct expr_parse *)array_get(&start_state->expr_parses, i);
        if (ep->parsed < array_size(&ep->expr->items)){
            struct expr_item *ei = (struct expr_item *)array_get(&ep->expr->items, ep->parsed);
            if(ei->sym == nonterm){
                parse_state_advance_expr_parse(state, ep);
            }
        }
    }       
}

struct ast_node *_build_ast(struct parse_states *states)
{
    
    return ast_node_new();
}

struct ast_node *parse(struct parser *parser, const char *text)
{
    struct tok tok;
    lexer_init(&parser->lexer, text);
    struct parse_states states;
    parse_states_init(&states);
    struct parse_state *state = 0;
    struct parse_state *next_state = 0;
    struct grammar *g = parser->grammar;
    //0. get the first token and jumpstart parsing process by initiating the start rule
    get_tok(&parser->lexer, &tok);
    state = parse_states_add_state(&states);
    struct rule *rule = hashtable_get_p(&parser->grammar->rule_map, parser->grammar->start_symbol);
    parse_state_init_rule(state, rule);
    while(tok.tok_type && state)
    {
        for (size_t i = 0; i < array_size(&state->expr_parses); i++) {
            struct expr_parse *ep = (struct expr_parse *)array_get(&state->expr_parses, i);
            if (ep->parsed == array_size(&ep->expr->items)){
                //this rule is parsed successfully. complete it and advance one step for all parents
                //from start state into current state.
                struct parse_state* start_state = (struct parse_state*)array_get(&states.states, ep->start_state_index);
                _complete(state, ep->rule->nonterm, start_state);

            }else{
                struct expr_item *ei = (struct expr_item *)array_get(&ep->expr->items, ep->parsed);
                if (ei->ei_type == EI_NONTERM && ei->sym != ep->rule->nonterm) {
                    // expects non-terminal, we're adding its rule exprs into current state
                    rule = hashtable_get_p(&g->rule_map, ei->sym);
                    parse_state_init_rule(state, rule);
                }else if(_is_match(&tok, ei)) /*scan and matched*/{
                    if(!next_state){
                        next_state = parse_states_add_state(&states);
                    }
                    parse_state_advance_expr_parse(next_state, ep);   
                } 
            }
        }
        get_tok(&parser->lexer, &tok);
        state = next_state;
        next_state = 0;
    }
    struct ast_node *ast = _build_ast(&states);
    parse_states_deinit(&states);
    return ast;
}