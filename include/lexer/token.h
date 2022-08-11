/*
 * token.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for token and opcode definition for recognizer/lexer
 */
#ifndef __MLANG_TOKEN_H__
#define __MLANG_TOKEN_H__

#include "lexer/source_location.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_TOKENTYPE(ENUM_ITEM) \
    ENUM_ITEM(TOKEN_EOF)             \
    ENUM_ITEM(TOKEN_EPSILON)         \
    ENUM_ITEM(TOKEN_INDENT)          \
    ENUM_ITEM(TOKEN_DEDENT)          \
    ENUM_ITEM(TOKEN_NEWLINE)         \
    ENUM_ITEM(TOKEN_INT)             \
    ENUM_ITEM(TOKEN_FLOAT)           \
    ENUM_ITEM(TOKEN_CHAR)            \
    ENUM_ITEM(TOKEN_STRING)          \
    ENUM_ITEM(TOKEN_IMPORT)          \
    ENUM_ITEM(TOKEN_EXTERN)          \
    ENUM_ITEM(TOKEN_TYPE)            \
    ENUM_ITEM(TOKEN_LET)             \
    ENUM_ITEM(TOKEN_IF)              \
    ENUM_ITEM(TOKEN_THEN)            \
    ENUM_ITEM(TOKEN_ELSE)            \
    ENUM_ITEM(TOKEN_TRUE)            \
    ENUM_ITEM(TOKEN_FALSE)           \
    ENUM_ITEM(TOKEN_IN)              \
    ENUM_ITEM(TOKEN_FOR)             \
    ENUM_ITEM(TOKEN_IDENT)           \
    ENUM_ITEM(TOKEN_LPAREN)          \
    ENUM_ITEM(TOKEN_RPAREN)          \
    ENUM_ITEM(TOKEN_LBRACKET)        \
    ENUM_ITEM(TOKEN_RBRACKET)        \
    ENUM_ITEM(TOKEN_LCBRACKET)       \
    ENUM_ITEM(TOKEN_RCBRACKET)       \
    ENUM_ITEM(TOKEN_COMMA)           \
    ENUM_ITEM(TOKEN_DOT)             \
    ENUM_ITEM(TOKEN_RANGE)           \
    ENUM_ITEM(TOKEN_VARIADIC)        \
    ENUM_ITEM(TOKEN_ASSIGN)          \
    ENUM_ITEM(TOKEN_ISTYPEOF)        \
    ENUM_ITEM(TOKEN_OP)

enum token_type { FOREACH_TOKENTYPE(GENERATE_ENUM) };

enum op_code{
    OP_NULL,
    OP_OR,//'||'
    OP_AND,
    OP_NOT,

    OP_BOR, //'|'
    OP_BAND, //'&'

    OP_EXPO,
    OP_TIMES,
    OP_DIVISION,
    OP_MODULUS,
    OP_PLUS,
    OP_MINUS,

    OP_LT,
    OP_LE,
    OP_EQ,
    OP_GT,
    OP_GE,
    OP_NE,

    OP_TOTAL //mark end of all tokens
};

#define TERMINAL_COUNT TOKEN_OP + OP_TOTAL
#define MAX_NONTERMS 1024
#define MAX_GRAMMAR_SYMBOLS TERMINAL_COUNT + MAX_NONTERMS

extern const char *token_type_strings[];

struct token_pattern{
    const char *name;       //c string name
    symbol symbol_name;     //symbol name
    const char *pattern;    //pattern
    struct re *re;          //regex for the pattern
    enum token_type token_type;
    enum op_code opcode; 
};

struct token_patterns{
    struct token_pattern *patterns;
    size_t pattern_count;
};

struct token {
    enum token_type token_type;
    struct source_location loc;
    union {
        string *str_val; //string literal
        double double_val; //double literal
        int int_val; //int literal
        char char_val; //char literal
        bool bool_val; //bool literal
        symbol symbol_val;
        enum op_code opcode;
    };
};

void token_init();

void token_deinit();

struct token_patterns get_token_patterns();

const char *get_opcode(enum op_code opcode);

symbol get_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode);

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode);

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type);

struct token_pattern *get_token_pattern_by_symbol(symbol symbol);

/*get symbol index, token or nonterm*/
u16 get_symbol_index(symbol symbol);
symbol get_symbol_by_index(u16 symbol_index);

/*get index by token*/
u16 get_token_index(enum token_type token_type, enum op_code opcode);

/*get total symbol count including terminal tokens and nonterm symbols*/
u16 get_symbol_count();

/*register nonterm symbol for grammar*/
u16 register_grammar_nonterm(symbol symbol);

bool is_terminal(u16 symbol_index);

#ifdef __cplusplus
}
#endif

#endif
