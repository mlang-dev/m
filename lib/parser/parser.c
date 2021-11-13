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
#include <assert.h>

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
    array_init(&state->complete_parses, sizeof(struct complete_parse));
}

void parse_state_deinit(struct parse_state *state)
{
    array_deinit(&state->expr_parses);
    array_deinit(&state->complete_parses);
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

void parse_state_add_completed_expr_parse(struct parse_state *state, struct expr_parse *ep, size_t complete_pos)
{
    struct complete_parse cp;
    cp.ep = ep;
    cp.complete_pos = complete_pos;
    array_push(&state->complete_parses, &cp);
}

struct complete_parse *parse_state_find_completed_expr_parse(struct parse_state *state, symbol nonterm, size_t complete_pos)
{
    struct complete_parse *cp;
    for(size_t i = 0; i < array_size(&state->complete_parses); i++){
        cp = (struct complete_parse*)array_get(&state->complete_parses, i);
        if (cp->ep->rule->nonterm == nonterm && cp->complete_pos == complete_pos){
            return cp;
        }
    }
    return 0;
}

struct complete_parse *parse_state_find_completed_expr_parse_except(struct parse_state *state, symbol nonterm, struct expr_parse *ep)
{
    struct complete_parse *cp = 0;
    size_t complete_parse_count = array_size(&state->complete_parses);
    for (size_t i = 0; i < complete_parse_count; i++) {
        struct complete_parse *cp = (struct complete_parse *)array_get(&state->complete_parses, complete_parse_count-i-1);
        if (cp->ep->rule->nonterm == nonterm && cp->ep != ep)
            return cp;
    }
    return 0;
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
        return expr_item_exists_symbol(ei, string_get(tok->tok_type)[0]);
    }    
}

void _complete(struct parse_state *state, struct expr_parse *complete_ep, struct parse_state *start_state)
{
    //finding in start_state, all parse expr's next (non-term) symbol is nonterm parameter passed
    //advance one and add to current state
    for (size_t i = 0; i < array_size(&start_state->expr_parses); i++) {
        struct expr_parse *s_ep = (struct expr_parse *)array_get(&start_state->expr_parses, i);
        if (s_ep->parsed < array_size(&s_ep->expr->items)){
            struct expr_item *ei = (struct expr_item *)array_get(&s_ep->expr->items, s_ep->parsed);
            if(ei->sym == complete_ep->rule->nonterm){
                parse_state_advance_expr_parse(state, s_ep);
            }
        }
    }
    //add completed ep into the start state
    parse_state_add_completed_expr_parse(start_state, complete_ep, state->state_index);
}

struct ast_node *_build_ast(struct parse_states *states, size_t from, struct complete_parse *cp)
{
    struct parse_state *state = array_get(&states->states, from);
    assert(state->state_index == from);   
    //cp->ep->expr
    size_t tok_pos = from;
    size_t item_count = array_size(&cp->ep->expr->items);
    struct ast_node *node = (cp->ep->expr->action.exp_item_index_count > 1) ? ast_node_new(cp->ep->expr->action.action) : 0;
    for(size_t i = 0; i < item_count; i++){
        if (expr_item_2_ast_node_index(cp->ep->expr, i) < 0){
            tok_pos++;
            continue;
        }
        struct expr_item *item = (struct expr_item *)array_get(&cp->ep->expr->items, i);
        struct ast_node *child = 0;
        state = (struct parse_state *)array_get(&states->states, tok_pos);
        if(item->ei_type){ //terminal
            child = ast_node_new(state->tok.tok_type);
            child->loc = state->tok.loc;
            tok_pos++;
        }else{ //noterminal
            struct complete_parse * child_cp = parse_state_find_completed_expr_parse_except(state, item->sym, cp->ep);
            if(child_cp){
                child = _build_ast(states, tok_pos, child_cp);
                tok_pos = child_cp->complete_pos;
            }
        }
        if(child){
            if(node)
                array_push(&node->children, &child);
            else
                return child;
        }
    }
    return node;
}

struct ast_node *parse(struct parser *parser, const char *text)
{
    lexer_init(&parser->lexer, text);
    struct parse_states states;
    parse_states_init(&states);
    struct parse_state *state = 0;
    struct parse_state *start_state = 0;
    struct parse_state *next_state = 0;
    struct grammar *g = parser->grammar;
    //0. get the first token and jumpstart parsing process by initiating the start rule
    state = parse_states_add_state(&states);
    start_state = state;
    get_tok(&parser->lexer, &state->tok);
    struct rule *rule = hashtable_get_p(&parser->grammar->rule_map, parser->grammar->start_symbol);
    parse_state_init_rule(state, rule);
    while(state)
    {
        for (size_t i = 0; i < array_size(&state->expr_parses); i++) {
            struct expr_parse *ep = (struct expr_parse *)array_get(&state->expr_parses, i);
            if (ep->parsed == array_size(&ep->expr->items)){
                //this rule is parsed successfully. complete it and advance one step for all parents
                //from start state into current state.
                struct parse_state* start_state = (struct parse_state*)array_get(&states.states, ep->start_state_index);
                _complete(state, ep, start_state);

            }else if(state->tok.tok_type){
                struct expr_item *ei = (struct expr_item *)array_get(&ep->expr->items, ep->parsed);
                if (ei->ei_type == EI_NONTERM && ei->sym != ep->rule->nonterm) {
                    // expects non-terminal, we're adding the rule's exprs into current state
                    // initial rule 
                    rule = hashtable_get_p(&g->rule_map, ei->sym);
                    parse_state_init_rule(state, rule);
                }else if(_is_match(&state->tok, ei)) /*scan and matched*/{
                    //terminal token match
                    if(!next_state){
                        next_state = parse_states_add_state(&states);
                    }
                    parse_state_advance_expr_parse(next_state, ep);   
                } 
            }
        }
        state = next_state;
        if (state)
            get_tok(&parser->lexer, &state->tok);
        next_state = 0;
    }
    size_t to = array_size(&states.states);

    struct complete_parse *cp = parse_state_find_completed_expr_parse(start_state, g->start_symbol, to-1);
    struct ast_node *ast = cp ? _build_ast(&states, 0, cp) : 0;
    parse_states_deinit(&states);
    return ast;
}