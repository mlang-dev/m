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
#include <ctype.h>

struct parser *parser_new(const char *grammar_text)
{
    struct grammar *grammar = grammar_parse(grammar_text);
    struct parser *parser;
    MALLOC(parser, sizeof(*parser));
    
    parser->grammar = grammar;
    parser->IDENT_TOKEN = to_symbol2("IDENT", 5);
    parser->NUM_TOKEN = to_symbol2("NUM", 3);
    parser->STRING_TOKEN = to_symbol2("STRING", 6);
    parser->CHAR_TOKEN = to_symbol2("CHAR", 4);
    return parser;
}


void parser_set_text(struct parser *parser, const char *text)
{
    parser->text = text;
    parser->text_pos = 0;
    parser->row_no = 1;
    parser->col_no = 1;
}

void parser_free(struct parser *parser)
{
    grammar_free(parser->grammar);
    free(parser);
}


//write non-null-terminated string: fwrite(string, sizeof(char), length, stdout);

void _move_ahead(struct parser *parser)
{
    if (!parser->text[parser->text_pos]) return;
    if (parser->text[parser->text_pos] == '\n') {
        parser->row_no++;
        parser->col_no = 1;
    } else {
        parser->col_no++;
    }
    parser->text_pos++;
}

void _scan_until(struct parser *parser, char until)
{
    do{
        _move_ahead(parser);

    } while (parser->text[parser->text_pos] != until);
}

void _scan_until_no_digit(struct parser *parser)
{
    const char *p = &parser->text[parser->text_pos];
    do {
        p++;
        _move_ahead(parser);
    } while (isdigit(*p) || *p == '.');
}

void _scan_until_no_space(struct parser *parser)
{
    do {
        _move_ahead(parser);
    } while (isspace(parser->text[parser->text_pos]));
}

void _scan_until_no_id(struct parser *parser)
{
    char ch;
    do
    {
        _move_ahead(parser);
        ch = parser->text[parser->text_pos];
    }
    while (ch == '_' || isalpha(ch) || isdigit(ch));
}

void _mark_token(struct parser *parser, struct tok *tok, symbol tok_type)
{
    tok->start_pos = parser->text_pos;
    tok->row_no = parser->row_no;
    tok->col_no = parser->col_no;
    tok->tok_type = tok_type;
}

void get_tok(struct parser *parser, struct tok *tok)
{
    tok->tok_type = 0;
    char ch = parser->text[parser->text_pos];
    switch (ch)
    {
    default:
        if(isspace(ch)){
            _scan_until_no_space(parser);
            get_tok(parser, tok);
        }
        else if(isdigit(ch) || 
            (ch == '.' && isdigit(parser->text[parser->text_pos + 1])) /*.123*/){
            _mark_token(parser, tok, parser->NUM_TOKEN);
            _scan_until_no_digit(parser);
        }
        else if(isalpha(ch) || ch == '_'){
            _mark_token(parser, tok, parser->IDENT_TOKEN);
            _scan_until_no_id(parser);
        }
        break;
    case '\0':
        break;
    case '#':
        _scan_until(parser, '\n');
        get_tok(parser, tok);
        break;
    case '\'':
        _mark_token(parser, tok, parser->CHAR_TOKEN);
        _scan_until(parser, '\'');
        _move_ahead(parser); //skip the single quote
        break;
    case '"':
        _mark_token(parser, tok, parser->STRING_TOKEN);
        _scan_until(parser, '"');
        _move_ahead(parser); // skip the double quote
        break;
    }
    if(tok->tok_type){
        tok->end_pos = parser->text_pos;
    }
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

bool parse(struct parser *parser, const char *text)
{
    struct tok tok;
    parser_set_text(parser, text);
    struct parse_states states;
    parse_states_init(&states);
    struct parse_state *state = 0;
    struct parse_state *next_state = 0;
    struct grammar *g = parser->grammar;
    //0. get the first token and jumpstart parsing process by initiating the start rule
    get_tok(parser, &tok);
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
        get_tok(parser, &tok);
        state = next_state;
        next_state = 0;
    }
    parse_states_deinit(&states);
    return true;
}