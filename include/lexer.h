/*
 * lexer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m lexer
 */
#ifndef __MLANG_LEXER_H__
#define __MLANG_LEXER_H__

#include "type.h"
#include "util.h"
#include <map>
#include <queue>
#include <string>


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
    ENUM_ITEM(TOKEN_LPAREN)          \
    ENUM_ITEM(TOKEN_RPAREN)          \
    ENUM_ITEM(TOKEN_LBRACKET)        \
    ENUM_ITEM(TOKEN_RBRACKET)        \
    ENUM_ITEM(TOKEN_OP)              \
    ENUM_ITEM(TOKEN_EOS)

enum TokenType { FOREACH_TOKENTYPE(GENERATE_ENUM) };

static const char* TokenTypeString[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

struct source_loc {
    int line;
    int col;
};

struct token {
    TokenType token_type;
    Type type;
    source_loc loc;
    int int_val;
    union {
        std::string* ident_str;
        double double_val;
    };
};

struct file_tokenizer {
    FILE* file;
    source_loc loc = { 1, 0 };
    source_loc tok_loc;
    token cur_token;
    token next_token;
    int curr_char = ' ';
    std::string ident_str;
};

file_tokenizer* create_tokenizer(FILE* file);
void destroy_tokenizer(file_tokenizer* tokenizer);
token& get_token(file_tokenizer* tokenizer);

#endif