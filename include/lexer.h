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
#include "clib/util.h"
#include "type.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TOKENTYPE(ENUM_ITEM) \
    ENUM_ITEM(TOKEN_UNK)             \
    ENUM_ITEM(TOKEN_EOF)             \
    ENUM_ITEM(TOKEN_IDENT)           \
    ENUM_ITEM(TOKEN_NUM)             \
    ENUM_ITEM(TOKEN_IMPORT)          \
    ENUM_ITEM(TOKEN_IF)              \
    ENUM_ITEM(TOKEN_THEN)            \
    ENUM_ITEM(TOKEN_ELSE)            \
    ENUM_ITEM(TOKEN_FOR)             \
    ENUM_ITEM(TOKEN_IN)              \
    ENUM_ITEM(TOKEN_RANGE)           \
    ENUM_ITEM(TOKEN_UNARY)           \
    ENUM_ITEM(TOKEN_BINARY)          \
    ENUM_ITEM(TOKEN_TRUE)            \
    ENUM_ITEM(TOKEN_FALSE)           \
    ENUM_ITEM(TOKEN_LPAREN)          \
    ENUM_ITEM(TOKEN_RPAREN)          \
    ENUM_ITEM(TOKEN_LBRACKET)        \
    ENUM_ITEM(TOKEN_RBRACKET)        \
    ENUM_ITEM(TOKEN_OP)              \
    ENUM_ITEM(TOKEN_EOS)

enum token_type { FOREACH_TOKENTYPE(GENERATE_ENUM) };

static const char* TokenTypeString[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

struct source_loc {
    int line;
    int col;
};

struct token {
    enum token_type token_type;
    enum type type;
    struct source_loc loc;
    int int_val;
    union {
        string* ident_str;
        double double_val;
    };
};

struct file_tokenizer {
    FILE* file;
    struct source_loc loc;
    struct source_loc tok_loc;
    struct token cur_token;
    struct token next_token;
    char curr_char[2];
    string ident_str;
};

struct file_tokenizer* create_tokenizer(FILE* file);
void destroy_tokenizer(struct file_tokenizer* tokenizer);
struct token* get_token(struct file_tokenizer* tokenizer);

#ifdef __cplusplus
}
#endif

#endif
