/*
 * lexer.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m lexer
 */
#ifndef __MLANG_LEXER_H__
#define __MLANG_LEXER_H__

#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/token.h"
#include "lexer/recognizer.h"
#include "lexer/source_location.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct lexer {
    const char *text;
    int pos;  //current text position
    int line;
    int col;
};

struct tok {
    struct source_location loc;
    enum token_type tok_type;
    symbol tok_type_name; // IDENT, NUM, STRING keywords like 'if', 'for' etc
};

extern symbol STRING_TOKEN;
extern symbol CHAR_TOKEN;
extern symbol NUM_TOKEN;
extern symbol IDENT_TOKEN;

void lexer_init(struct lexer *lexer, const char *text);
void get_tok(struct lexer *lexer, struct tok *tok);

#ifdef __cplusplus
}
#endif

#endif
