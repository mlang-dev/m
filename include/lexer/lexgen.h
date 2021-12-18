/*
 * lexgen.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for a lexer generator, it takes the input of token pattern match list
 */
#ifndef __MLANG_LEXGEN_H__
#define __MLANG_LEXGEN_H__

#include "clib/string.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct lexgen {
    string lex_hdr;
    string lex_impl;
};


void lexer_init(struct lexer *lexer, const char *text);
string to_lexer(struct lexer *lexer, struct token *tok);

#ifdef __cplusplus
}
#endif

#endif
