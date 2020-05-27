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
    ENUM_ITEM(TOKEN_EOL)             \
    ENUM_ITEM(TOKEN_IDENT)           \
    ENUM_ITEM(TOKEN_NUM)             \
    ENUM_ITEM(TOKEN_CHAR)            \
    ENUM_ITEM(TOKEN_STRING)          \
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
    ENUM_ITEM(TOKEN_VARIADIC)        \
    ENUM_ITEM(TOKEN_OP)              \
    ENUM_ITEM(TOKEN_OR)              \
    ENUM_ITEM(TOKEN_AND)             \
    ENUM_ITEM(TOKEN_NOT)             \
    ENUM_ITEM(TOKEN_NEG)             \
    ENUM_ITEM(TOKEN_LT)              \
    ENUM_ITEM(TOKEN_LE)              \
    ENUM_ITEM(TOKEN_EQ)              \
    ENUM_ITEM(TOKEN_NEQ)             \
    ENUM_ITEM(TOKEN_GT)              \
    ENUM_ITEM(TOKEN_GE)              \
    ENUM_ITEM(TOKEN_ADD)             \
    ENUM_ITEM(TOKEN_SUB)             \
    ENUM_ITEM(TOKEN_MUL)             \
    ENUM_ITEM(TOKEN_DIV)             

enum token_type { FOREACH_TOKENTYPE(GENERATE_ENUM) };

#define IS_OP(type)     (type >= TOKEN_OP)

extern const char* token_type_strings[];

struct source_loc {
    int line;
    int col;
};

struct token {
    enum token_type token_type;
    enum type type;
    struct source_loc loc;
    union {
        string* str_val;
        double double_val;
        int int_val;
        char char_val;
    };
};

struct file_tokenizer {
    FILE* file;
    const char* filename;
    struct source_loc loc;
    struct source_loc tok_loc;
    struct token cur_token;
    struct token next_token;
    char curr_char[2];
    string str_val;
};

struct file_tokenizer* create_tokenizer(FILE* file, const char* filename);
void destroy_tokenizer(struct file_tokenizer* tokenizer);
struct token* get_token(struct file_tokenizer* tokenizer);
extern const char* boolean_values[2];

#ifdef __cplusplus
}
#endif

#endif
