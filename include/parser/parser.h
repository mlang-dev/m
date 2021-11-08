/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for a generic parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include "parser/grammar.h"
#include "clib/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tok {
    int start_pos;
    int end_pos;
    int row_no; // 1-based
    int col_no; // 1-based
    symbol tok_type; // ID, NUM, STRING keywords like 'if', 'for' etc
};

struct parser {
    struct grammar *grammar;

    const char *text;
    int text_pos;  //current text position
    int row_no;
    int col_no;

    symbol STRING_TOKEN;
    symbol CHAR_TOKEN;
    symbol NUM_TOKEN;
    symbol ID_TOKEN;
};

struct expr_parse{
    struct expr *expr; //rule expression applied in current position of tokens
    int parsed;  //number of expr in the rule parsed, or pointing to the next rule expr to be parse for each expression in the rule.
    size_t start_state_index; //0-based index of parse state
};

struct rule_parse {
    struct rule *rule; // rule expression applied in current position of tokens
    struct array expr_parses; // number of expr in the rule parsed, or pointing to the next rule expr to be parse for each expression in the rule.
};

// for each position of tokens we maintain a partial parse state: n tokens will have n parse states
struct parse_state{
    size_t state_index;   
    struct array rule_parses; //number of parses for current position of tokens
};

struct parse_states {
    struct array states; // number of parse state for current position of tokens
};

void get_tok(struct parser *parser, struct tok *tok);
struct parser *parser_new(const char *grammar);
void parser_free(struct parser *parser);
bool parser_parse(struct parser *parser, const char *text);
void parser_set_text(struct parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif
