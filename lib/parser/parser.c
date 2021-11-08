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
    parser->ID_TOKEN = to_symbol2("ID", 2);
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
            _mark_token(parser, tok, parser->ID_TOKEN);
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

void rule_parse_init(struct rule_parse *rp, struct rule *rule, bool init_expr_parse, size_t state_index)
{
    rp->rule = rule;
    array_init(&rp->expr_parses, sizeof(struct expr_parse));
    if (init_expr_parse){
        struct expr_parse ep;
        ep.parsed = 0;
        ep.start_state_index = state_index;
        for(size_t i = 0; i < array_size(&rule->exprs); i++){
            ep.expr = (struct expr *)array_get(&rule->exprs, i);
            array_push(&rp->expr_parses, &ep);
        }
    }
}

void rule_parse_deinit(struct rule_parse *rp)
{
    array_deinit(&rp->expr_parses);
}

void parse_state_init(struct parse_state *state, int state_index)
{
    state->state_index = state_index;
    array_init(&state->rule_parses, sizeof(struct rule_parse));
}

struct rule_parse *parse_state_find_rule_parse(struct parse_state *state, symbol nonterm)
{
    for(size_t i = 0; i < array_size(&state->rule_parses); i++){
        struct rule_parse *rp = (struct rule_parse *)array_get(&state->rule_parses, i);
        if(rp->rule->nonterm == nonterm){
            return rp;
        }
    }
    return 0;
}

void parse_state_deinit(struct parse_state *state)
{
    for (size_t i = 0; i < array_size(&state->rule_parses); i++) {
        struct rule_parse *rp = (struct rule_parse *)array_get(&state->rule_parses, i);
        rule_parse_deinit(rp);
    }
    array_deinit(&state->rule_parses);
}

struct rule_parse *parse_state_add_rule(struct parse_state *state, struct rule *rule)
{
    struct rule_parse rp;
    rule_parse_init(&rp, rule, true, state->state_index);
    array_push(&state->rule_parses, &rp);
    return (struct rule_parse *)array_back(&state->rule_parses);
}

struct expr_parse *parse_state_advance_expr_parse_to_state(struct parse_state *state, struct rule *rule, struct expr_parse *ep)
{
    struct rule_parse *rp = parse_state_find_rule_parse(state, rule->nonterm);
    if (!rp){
        struct rule_parse init_rp;
        rule_parse_init(&init_rp, rule, false, 0);
        array_push(&state->rule_parses, &init_rp);
        rp = (struct rule_parse *)array_back(&state->rule_parses);
    }
    array_push(&rp->expr_parses, ep);
    return (struct expr_parse *)array_back(&rp->expr_parses);
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

void _predict(struct parser *parser, struct parse_state *state, symbol sym)
{
    struct rule_parse *rp = 0;
    struct rule *rule = hashtable_get_p(&parser->grammar->rule_map, sym);
    rp = parse_state_add_rule(state, rule);
    for (size_t i = 0; i < array_size(&rp->expr_parses); i++) {
        struct expr_parse *ep = (struct expr_parse *)array_get(&rp->expr_parses, i);
        struct expr_item *ei = (struct expr_item *)array_get(&ep->expr->items, ep->parsed);
        if (ei->ei_type == EI_NONTERM && ei->sym != rp->rule->nonterm && !parse_state_find_rule_parse(state, ei->sym)) {
            // this is new non-terminal, we're adding to current state
            _predict(parser, state, ei->sym);
        }
    }
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

struct parse_state *_scan(struct parse_states *states, struct parse_state *state, struct tok *tok)
{
    struct parse_state *next_state = 0;
    for (size_t j = 0; j < array_size(&state->rule_parses); j++){
        struct rule_parse *rp = (struct rule_parse *)array_get(&state->rule_parses, j);
        for (size_t i = 0; i < array_size(&rp->expr_parses); i++)
        {
            struct expr_parse *ep = (struct expr_parse *)array_get(&rp->expr_parses, i);
            struct expr_item *ei = (struct expr_item *)array_get(&ep->expr->items, ep->parsed);
            if (_is_match(tok, ei)) {
                // this token is matched terminal, we're adding to next state
                if(!next_state){
                    next_state = parse_states_add_state(states);
                }
                // now copy the expr parse to the next state and advance the parsed location
                ep = parse_state_advance_expr_parse_to_state(next_state, rp->rule, ep);
                ep->parsed ++;
            }
        }
    }
    return next_state;
}

void _complete(struct parse_state *state)
{
    (void)state;
}

bool parser_parse(struct parser *parser, const char *text)
{
    struct tok tok;
    parser_set_text(parser, text);
    struct parse_states states;
    parse_states_init(&states);
    struct parse_state *state = 0;
    get_tok(parser, &tok);
    symbol sym = parser->grammar->start_symbol;
    while(tok.tok_type)
    {
        state = parse_states_add_state(&states);
        //1: predict
        _predict(parser, state, sym);
        //2. scan and move to next state
        state = _scan(&states, state, &tok);
        //3. complete
        _complete(state);

        get_tok(parser, &tok);
    }
    parse_states_deinit(&states);
    return true;
}