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
#include "lexer/keyword.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TOKENTYPE(ENUM_ITEM) \
    ENUM_ITEM(TOKEN_NULL)            \
    ENUM_ITEM(TOKEN_EOF)             \
    ENUM_ITEM(TOKEN_NEWLINE)         \
    ENUM_ITEM(TOKEN_IDENT)           \
    ENUM_ITEM(TOKEN_INT)             \
    ENUM_ITEM(TOKEN_FLOAT)           \
    ENUM_ITEM(TOKEN_CHAR)            \
    ENUM_ITEM(TOKEN_STRING)          \
    ENUM_ITEM(TOKEN_SYMBOL)          \
    ENUM_ITEM(TOKEN_TOTAL)

enum token_type { FOREACH_TOKENTYPE(GENERATE_ENUM) };

extern const char *token_type_strings[];

struct source_loc {
    int line;
    int col;
};

struct token {
    enum token_type token_type;
    struct source_loc loc;
    union {
        string *str_val; //string literal
        double double_val; //double literal
        int int_val; //int literal
        char char_val; //char literal
        symbol symbol_val;
    }val;
};

struct tokenizer {
    FILE *file;
    const char *filename;
    struct source_loc loc;
    struct source_loc tok_loc;
    struct token cur_token;
    char curr_char[2];
    string str_val;
    char peek;
    struct keyword_states keyword_states;
    struct hashtable keyword_2_tokens;
};

struct tokenizer *create_tokenizer(FILE *file, const char *filename, const char **keyword_symbols, int keyword_count);
struct tokenizer *create_tokenizer_for_string(const char *content, const char **keyword_symbols, int keyword_count);
void destroy_tokenizer(struct tokenizer *tokenizer);
struct token *get_token(struct tokenizer *tokenizer);
void init_token(struct token *token);
extern const char *boolean_values[2];

#ifdef __cplusplus
}
#endif

#endif
