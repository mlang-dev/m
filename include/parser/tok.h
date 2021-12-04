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
#include "lexer/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tok {
    struct source_location loc;
    symbol tok_type; // IDENT, NUM, STRING keywords like 'if', 'for' etc
};

extern symbol STRING_TOKEN;
extern symbol CHAR_TOKEN;
extern symbol NUM_TOKEN;
extern symbol IDENT_TOKEN;

struct lexer {
    const char *text;
    int pos;  //current text position
    int line;
    int col;
};

void lexer_init(struct lexer *lexer, const char *text);
void get_tok(struct lexer *lexer, struct tok *tok);

#ifdef __cplusplus
}
#endif

#endif
