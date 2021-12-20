/*
 * lexer.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m lexer
 */
#ifndef __MLANG_M_LEXER_H__
#define __MLANG_M_LEXER_H__

#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/token.h"
#include "lexer/recognizer.h"
#include "lexer/source_location.h"
#include "parser/m_grammar.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tokenizer {
    FILE *file;
    const char *filename;
    struct source_location loc;
    struct source_location tok_loc;
    struct token cur_token;
    char curr_char[2];
    string str_val;
    char peek;
    struct rcg_states rcg_states;
};

struct tokenizer *create_tokenizer(FILE *file, const char *filename);
struct tokenizer *create_tokenizer_for_string(const char *content);
void destroy_tokenizer(struct tokenizer *tokenizer);
struct token *get_token(struct tokenizer *tokenizer);
extern const char *boolean_values[2];

#ifdef __cplusplus
}
#endif

#endif
