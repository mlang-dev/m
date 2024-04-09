/*
 * token.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for token definitions
 */
#ifndef __TOKEN_H__
#define __TOKEN_H__

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
    #include "lexer/m/m_token.keyword"
    
};

enum op_code {
    OP_NULL,
 
    #include "lexer/m/m_token.operator"
    
    OP_TOTAL // EQUALS to THE NUMBER OF LANGUAGE TOKENS + 1
};

struct token_pattern{
    const char *token_name;       //c string name
    const char *pattern;    //pattern
    enum token_type token_type;
    enum op_code opcode; 

    const char *style_class_name; //ui style class name, used for syntax highlight
    symbol symbol_name;     //symbol name
    struct re *re;          //regex for the pattern
};

#define TERMINAL_COUNT (TOKEN_OP + OP_TOTAL)

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

void token_init(void);

void token_deinit(void);

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode);

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type);

struct token_patterns get_token_patterns(void);

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

#define is_comment_token(tp) (tp == TOKEN_PYCOMMENT || tp == TOKEN_LINECOMMENT || tp == TOKEN_BLOCKCOMMENT)
#define is_linecomment_token(tp) (tp == TOKEN_PYCOMMENT || tp == TOKEN_LINECOMMENT)

enum op_code get_op_code_from_assign_op(enum op_code assign_op);

void tok_clean(struct token *tok);

#ifdef __cplusplus
}
#endif

#endif
