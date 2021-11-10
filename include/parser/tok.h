/*
 * tok.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for a generic tokenizer
 */

#ifndef __MLANG_TOKEN_H__
#define __MLANG_TOKEN_H__

#include "clib/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tok {
    int start_pos;
    int end_pos;
    int row_no; // 1-based
    int col_no; // 1-based
    union{
        symbol tok_type; // IDENT, NUM, STRING keywords like 'if', 'for' etc
        char char_type;
    };
};

struct lexer {
    const char *text;
    int text_pos;  //current text position
    int row_no;
    int col_no;

    symbol STRING_TOKEN;
    symbol CHAR_TOKEN;
    symbol NUM_TOKEN;
    symbol IDENT_TOKEN;
};

void lexer_init(struct lexer *lexer, const char *text);
void get_tok(struct lexer *lexer, struct tok *tok);

#ifdef __cplusplus
}
#endif

#endif
