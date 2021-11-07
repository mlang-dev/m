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

void get_tok(struct parser *parser, struct tok *tok);

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

struct parser *parser_new(const char *grammar);
void parser_free(struct parser *parser);
bool parser_parse(struct parser *parser, const char *text);
void parser_set_text(struct parser *parser, const char *text);

#ifdef __cplusplus
}
#endif

#endif
