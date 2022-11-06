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


struct token_pattern *get_token_pattern_by_symbol(symbol symbol);

/*get symbol index, token or nonterm*/
symbol get_symbol_by_index(u16 symbol_index);

/*get index by token*/
u16 get_token_index(enum token_type token_type, enum op_code opcode);


#ifdef __cplusplus
}
#endif

#endif
