/*
 * terminal.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for terminal token definitions
 */
#ifndef __MLANG_TERMINAL_H__
#define __MLANG_TERMINAL_H__

#include "lexer/source_location.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum token_type{
    TOKEN_NULL,
    TOKEN_EOF,
    TOKEN_EPSILON,
    TOKEN_INDENT,
    TOKEN_DEDENT,

    TOKEN_NEWLINE,
    TOKEN_INT,
    TOKEN_DOUBLE,
    TOKEN_COMPLEX,
    TOKEN_CHAR,

    TOKEN_STRING,
    TOKEN_FROM,
    TOKEN_IMPORT,
    TOKEN_MEMORY,
    TOKEN_EXTERN,

    TOKEN_ENUM,
    TOKEN_STRUCT,
    TOKEN_UNION,
    TOKEN_TYPE,
    TOKEN_LET,

    TOKEN_VAR,
    TOKEN_MUT,
    TOKEN_FUN,
    TOKEN_MAPTO,
    TOKEN_IF,
    TOKEN_THEN,

    TOKEN_ELSE,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_IN,
    TOKEN_FOR,

    TOKEN_WHILE,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_UNIT,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LCBRACKET,
    TOKEN_RCBRACKET,

    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_RANGE,
    TOKEN_VARIADIC,
    TOKEN_ISTYPEOF,

    TOKEN_DO,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_RETURN,
    
    TOKEN_YIELD,
    TOKEN_ASYNC,
    TOKEN_AWAIT,
    TOKEN_MATCH,
    TOKEN_WITH,
    
    /*IDENT token has to been at the end of keyword list, so that exact keyword match 
      always happens before identifier match
    */
    TOKEN_IDENT, 
    TOKEN_OP,
};

enum op_code {
    OP_NULL,
    OP_DOT, 
    OP_OR, //'||'
    OP_AND, //'&&'
    OP_NOT, // !

    OP_BITNOT, // '~'
    OP_BITOR, //'|'
    OP_BITEXOR, //'^' exclusive or
    OP_BAND, //'&' bit and or reference operator
    OP_BSL, //<< shift left
    OP_BSR, //>> shift right

    OP_SQRT, //|/
    OP_POW,
    OP_STAR, //used as multiplication operator or dereference operator
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

    OP_COND, //? conditional check

    OP_ASSIGN, //=
    OP_MUL_ASSN, //  *=
    OP_DIV_ASSN, //  /=
    OP_MOD_ASSN, //  %=
    OP_ADD_ASSN, //  +=
    OP_SUB_ASSN, //  -=
    OP_LEFT_ASSN, //  <<=
    OP_RIGHT_ASSN, //  >>=
    OP_AND_ASSN, //  &=
    OP_XOR_ASSN, //  ^=
    OP_OR_ASSN, //  |=

    OP_INC, // ++
    OP_DEC, // --

    OP_TOTAL // mark end of all tokens
};

#define TERMINAL_COUNT TOKEN_OP + OP_TOTAL
#define MAX_NONTERMS 2048
#define MAX_GRAMMAR_SYMBOLS TERMINAL_COUNT + MAX_NONTERMS

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
        const char *str_val; //string literal
        f64 double_val; //f64 literal
        int int_val; //int literal
        symbol symbol_val;
        enum op_code opcode;
    };
};

void terminal_init();

void terminal_deinit();

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode);

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type);

extern struct token_pattern g_token_patterns[TERMINAL_COUNT];

struct token_patterns get_token_patterns();

symbol get_terminal_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode);

/*get index by token*/
u16 get_terminal_token_index(enum token_type token_type, enum op_code opcode);

const char *get_opcode(enum op_code opcode);

bool is_terminal(u16 symbol_index);

bool is_relational_op(enum op_code opcode);

#define is_open_group(tp) (tp == TOKEN_LPAREN || tp == TOKEN_LBRACKET || tp == TOKEN_LCBRACKET)
#define is_close_group(tp) (tp == TOKEN_RPAREN || tp == TOKEN_RBRACKET || tp == TOKEN_RCBRACKET)
#define is_match_open(open, close) ((open == TOKEN_LPAREN && close == TOKEN_RPAREN) || (open == TOKEN_LBRACKET && close == TOKEN_RBRACKET) || (open == TOKEN_LCBRACKET && close == TOKEN_RCBRACKET))
#define is_assign_op_sugar(tp) (tp >= OP_MUL_ASSN && tp <= OP_OR_ASSN)

enum op_code get_op_code_from_assign_op(enum op_code assign_op);

void tok_clean(struct token *tok);

#ifdef __cplusplus
}
#endif

#endif
