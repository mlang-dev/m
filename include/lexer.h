/*
 * lexer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m lexer
 */
#ifndef __MLANG_LEXER_H__
#define __MLANG_LEXER_H__

#include <stdio.h>
#include "type.h"
#include "clib/util.h"
#include "clib/string.h"

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

enum _TokenType { FOREACH_TOKENTYPE(GENERATE_ENUM) };
typedef enum _TokenType TokenType;

static const char* TokenTypeString[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

typedef struct _source_loc {
    int line;
    int col;
}source_loc;

typedef struct _token {
    TokenType token_type;
    Type type;
    source_loc loc;
    int int_val;
    union {
        string* ident_str;
        double double_val;
    };
}token;

typedef struct _file_tokenizer {
    FILE* file;
    source_loc loc;
    source_loc tok_loc;
    token cur_token;
    token next_token;
    char curr_char[2];
    string ident_str;
}file_tokenizer;

file_tokenizer* create_tokenizer(FILE* file);
void destroy_tokenizer(file_tokenizer* tokenizer);
token* get_token(file_tokenizer* tokenizer);

#ifdef __cplusplus
}
#endif

#endif
