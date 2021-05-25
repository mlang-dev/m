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
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TOKENTYPE(ENUM_ITEM) \
    ENUM_ITEM(TOKEN_NULL)            \
    ENUM_ITEM(TOKEN_EOF)             \
    ENUM_ITEM(TOKEN_EOL)             \
    ENUM_ITEM(TOKEN_IDENT)           \
    ENUM_ITEM(TOKEN_INT)             \
    ENUM_ITEM(TOKEN_FLOAT)           \
    ENUM_ITEM(TOKEN_CHAR)            \
    ENUM_ITEM(TOKEN_STRING)          \
    ENUM_ITEM(TOKEN_KEYWORD)         \
    ENUM_ITEM(TOKEN_OP)              \
    ENUM_ITEM(TOKEN_TOTAL)

enum token_type { FOREACH_TOKENTYPE(GENERATE_ENUM) };

#define IS_OP(type) (type == TOKEN_OP)

extern const char *token_type_strings[];

struct source_loc {
    int line;
    int col;
};

struct token {
    enum token_type token_type;
    struct source_loc loc;
    union {
        string *str_val;
        double double_val;
        int int_val;
        char char_val;
    };
    symbol keyword;
};

struct file_tokenizer {
    FILE *file;
    const char *filename;
    struct source_loc loc;
    struct source_loc tok_loc;
    struct token cur_token;
    char curr_char[2];
    string str_val;
    int peek;
};

struct file_tokenizer *create_tokenizer(FILE *file, const char *filename);
void destroy_tokenizer(struct file_tokenizer *tokenizer);
struct token *get_token(struct file_tokenizer *tokenizer);
void init_token(struct token *token);
extern const char *boolean_values[2];

void lexer_init();
void lexer_deinit();

#ifdef __cplusplus
}
#endif

#endif
