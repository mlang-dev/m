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
#include "lexer/terminal.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct token {
    enum token_type token_type;
    struct source_location loc;
    union {
        string *str_val; //string literal
        double double_val; //double literal
        int int_val; //int literal
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

bool is_assign(enum op_code opcode);

bool is_relational_op(enum op_code opcode);

#define is_open_group(tp) (tp == TOKEN_LPAREN || tp == TOKEN_LBRACKET || tp == TOKEN_LCBRACKET)
#define is_close_group(tp) (tp == TOKEN_RPAREN || tp == TOKEN_RBRACKET || tp == TOKEN_RCBRACKET)
#define is_match_open(open, close) ((open == TOKEN_LPAREN && close == TOKEN_RPAREN) || (open == TOKEN_LBRACKET && close == TOKEN_RBRACKET) || (open == TOKEN_LCBRACKET && close == TOKEN_RCBRACKET))

#ifdef __cplusplus
}
#endif

#endif
