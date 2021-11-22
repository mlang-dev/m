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
#include "clib/stack.h"
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
    FREE(parser);
}

struct expr_parse *parse_state_find_expr_parse(struct parse_state *state, struct expr* expr, size_t parsed)
{
    for(size_t i = 0; i < array_size(&state->expr_parses); i++){
        struct expr_parse *ep = *(struct expr_parse **)array_get(&state->expr_parses, i);
        if(ep->expr == expr && ep->parsed == parsed){
            return ep;
        }
    }
    return 0;
}

struct expr_parse *expr_parse_new(size_t start_state_index, size_t parsed, struct rule *rule)
{
    struct expr_parse *ep;
    MALLOC(ep, sizeof(*ep));
    ep->start_state_index = start_state_index;
    ep->parsed = parsed;
    ep->rule = rule;
    return ep;
}

struct complete_parse *complete_parse_new(size_t end_state_index, struct expr_parse *ep)
{
    struct complete_parse *cp;
    MALLOC(cp, sizeof(*cp));
    cp->end_state_index = end_state_index;
    cp->ep = ep;
    return cp;
}

void parse_state_init_rule(struct parse_state *state, struct rule *rule)
{
    for(size_t i = 0; i < array_size(&rule->exprs); i++){
        struct expr_parse* ep = expr_parse_new(state->state_index, 0, rule);
        ep->expr = (struct expr *)array_get(&rule->exprs, i);
        if (!parse_state_find_expr_parse(state, ep->expr, 0)){
            array_push(&state->expr_parses, &ep);
        }
    }
}

void parse_state_init(struct parse_state *state, int state_index)
{
    state->state_index = state_index;
    array_init_free(&state->expr_parses, sizeof(struct expr_parse *), (free_fun)free);
    hashtable_init_with_value_size(&state->complete_parses, 0, (free_fun)array_free);//
}

void parse_state_deinit(struct parse_state *state)
{
    array_deinit(&state->expr_parses);
    hashtable_deinit(&state->complete_parses);
}

void parse_state_advance_expr_parse(struct parse_state *state, struct expr_parse *ep)
{
    if (!parse_state_find_expr_parse(state, ep->expr, ep->parsed + 1)){
        struct expr_parse *init_ep = expr_parse_new(ep->start_state_index, ep->parsed + 1, ep->rule);
        init_ep->expr = ep->expr;
        array_push(&state->expr_parses, &init_ep);
    }
}

void parse_state_add_completed_expr_parse(struct parse_state *state, struct expr_parse *ep, size_t end_state_index)
{
    struct array *cps = (struct array*)hashtable_get_p(&state->complete_parses, ep->rule->nonterm);
    if(!cps){
        cps = array_new(sizeof(struct complete_parse*));
        hashtable_set_p(&state->complete_parses, ep->rule->nonterm, cps);
    }
    struct complete_parse *cp = complete_parse_new(end_state_index, ep);
    array_push(cps, &cp);
}

struct complete_parse *parse_state_find_completed_expr_parse(struct parse_state *state, symbol nonterm, size_t end_state_index)
{
    struct complete_parse *cp;
    struct array *cps = hashtable_get_p(&state->complete_parses, nonterm);
    size_t cp_count = array_size(cps);
    for(size_t i = 0; i < cp_count; i++){
        size_t j = cp_count - 1 - i;
        cp = *(struct complete_parse**)array_get(cps, j);
        if (cp->ep->rule->nonterm == nonterm && cp->end_state_index == end_state_index){
            return cp;
        }
    }
    return 0;
}

void parse_state_find_child_completed_expr_parse(struct parse_state *state, symbol nonterm, struct complete_parse *parent, struct array *children)
{
    struct array *cps = hashtable_get_p(&state->complete_parses, nonterm);
    if (!cps) return;
    size_t complete_parse_count = array_size(cps);
    for (size_t i = 0; i < complete_parse_count; i++) {
        size_t j = complete_parse_count-1-i;
        struct complete_parse *cp = *(struct complete_parse **)array_get(cps, j);
        if (cp->ep->rule->nonterm == nonterm && cp != parent && cp->end_state_index <= parent->end_state_index){
            array_push(children, &cp);
        }
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
        return expr_item_exists_symbol(ei, string_get(tok->tok_type)[0]);
    }    
}

void _complete(struct parse_state *state, struct expr_parse *complete_ep, struct parse_state *start_state)
{
    //finding in start_state, all parse expr's next (non-term) symbol is nonterm parameter passed
    //advance one and add to current state
    size_t ep_count = array_size(&start_state->expr_parses);
    for (size_t i = 0; i < ep_count; i++) {
        struct expr_parse *s_ep = *(struct expr_parse **)array_get(&start_state->expr_parses, i);
        if (s_ep->parsed < array_size(&s_ep->expr->items)){
            struct expr_item *ei = (struct expr_item *)array_get(&s_ep->expr->items, s_ep->parsed);
            if(ei->sym == complete_ep->rule->nonterm){
                parse_state_advance_expr_parse(state, s_ep);
            }
        }
    }
    //add completed ep into the start state
    // string rule = print_rule_expr(complete_ep->rule->nonterm, complete_ep->expr);
    // printf("complete rule: %.*s, [%d, %d)\n", (int)string_size(&rule), string_get(&rule), 
    //     (int)start_state->state_index, (int)state->state_index);
    parse_state_add_completed_expr_parse(start_state, complete_ep, state->state_index);
}

struct _child_cp_call{
    size_t state_index;
    size_t expr_item_index;
    struct complete_parse *child_cp;
};

struct _child_parse{
    struct parse_state *state;
    enum expr_item_type ei_type;
    struct complete_parse *child_cp;
};

struct ast_node *_build_ast(struct parse_states *states, size_t from, struct complete_parse *cp)
{
    struct parse_state *state = array_get(&states->states, from);
    assert(state->state_index == from);   
    size_t state_index = from;
    size_t item_count = array_size(&cp->ep->expr->items);
    struct stack s;
    struct _child_cp_call child_cp_call;
    stack_init(&s, sizeof(child_cp_call));
    struct array child_parses;
    struct _child_parse child_parse;
    struct expr_item *item = 0;
    struct complete_parse *child_cp = 0;
    array_init(&child_parses, sizeof(child_parse));
    for(size_t i = 0; i < item_count; i++){
        if(item && !item->ei_type && !child_cp){
            struct _child_cp_call *ccp = (struct _child_cp_call *)stack_pop(&s);
            assert(ccp);
            i = ccp->expr_item_index;
            state_index = ccp->state_index;
            child_cp = ccp->child_cp;
            assert(child_cp);
        }else{
            child_cp = 0;
        }
        state = (struct parse_state *)array_get(&states->states, state_index);
        item = (struct expr_item *)array_get(&cp->ep->expr->items, i);
        if(item->ei_type){ //terminal
            state_index++;
        }else{ //noterminal
            if(!child_cp){
                struct array children;
                array_init(&children, sizeof(struct complete_parse*));
                parse_state_find_child_completed_expr_parse(state, item->sym, cp, &children);
                if(array_size(&children)){
                    child_cp = *(struct complete_parse**)array_get(&children, 0);
                    child_cp_call.expr_item_index = i;
                    child_cp_call.state_index = state_index;
                    for (size_t j = 1; j < array_size(&children); j++){
                        child_cp_call.child_cp = *(struct complete_parse**)array_get(&children, j);
                        stack_push(&s, &child_cp_call);
                    }
                }
                array_deinit(&children);
            }
            if(child_cp){
                child_parse.child_cp = child_cp;
                state_index = child_cp->end_state_index;
            }else{
                if (i==item_count - 1) i--;
            }
        }
        child_parse.ei_type = item->ei_type;
        child_parse.state = state;
        if(i<array_size(&child_parses)){
            array_set(&child_parses, i, &child_parse);
        }else{
            array_push(&child_parses, &child_parse);
        }
    }
    
    assert(state_index == cp->end_state_index);
    //build ast
    struct ast_node *node = (cp->ep->expr->action.exp_item_index_count > 1) ? ast_node_new(cp->ep->expr->action.action) : 0;
    for(size_t i = 0; i < array_size(&child_parses); i++){
        struct _child_parse *c_p = (struct _child_parse *)array_get(&child_parses, i);
        struct ast_node *child = 0; 
        if (expr_item_2_ast_node_index(cp->ep->expr, i) < 0){
            continue;
        }
        if(c_p->ei_type){ //terminal
            child = ast_node_new(c_p->state->tok.tok_type);
            child->loc = c_p->state->tok.loc;
        }else{ //noterminal
            child = _build_ast(states, c_p->state->state_index, c_p->child_cp);
        }
        if(node){
            array_push(&node->children, &child);
        }
        else{
            node = child;
            break;
        }
    }

    stack_deinit(&s);
    array_deinit(&child_parses);
    return node;
}

struct ast_node *parse(struct parser *parser, const char *text)
{
    lexer_init(&parser->lexer, text);
    struct parse_states states;
    parse_states_init(&states);
    struct parse_state *state = 0;
    struct parse_state *next_state = 0;
    struct grammar *g = parser->grammar;
    //0. get the first token and jumpstart parsing process by initiating the start rule
    state = parse_states_add_state(&states);
    get_tok(&parser->lexer, &state->tok);
    struct rule *rule = hashtable_get_p(&parser->grammar->rule_map, parser->grammar->start_symbol);
    parse_state_init_rule(state, rule);
    while(state)
    {
        for (size_t i = 0; i < array_size(&state->expr_parses); i++) {
            struct expr_parse *ep = *(struct expr_parse **)array_get(&state->expr_parses, i);
            assert(array_size(&ep->expr->items) > 0);
            if (ep->parsed == array_size(&ep->expr->items)){
                //this rule is parsed successfully. complete it and advance one step for all parents
                //from start state into current state.
                struct parse_state* ss = (struct parse_state*)array_get(&states.states, ep->start_state_index);
                _complete(state, ep, ss);

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
    if(!to) return 0;
    struct parse_state *start_state = (struct parse_state *)array_get(&states.states, 0);
    struct complete_parse *cp = parse_state_find_completed_expr_parse(start_state, g->start_symbol, to-1);
    struct ast_node *ast = cp ? _build_ast(&states, 0, cp) : 0;
    parse_states_deinit(&states);
    return ast;
}