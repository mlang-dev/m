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
    TOKEN_NULL,
    TOKEN_EOF,
    TOKEN_EPSILON,

	TOKEN_INDENT,
	TOKEN_DEDENT,
	TOKEN_LITERAL_CHAR,
	TOKEN_LITERAL_STRING,
	TOKEN_NEWLINE,

	TOKEN_LITERAL_INT,
	TOKEN_LITERAL_FLOAT,
	TOKEN_LITERAL_COMPLEX,
	TOKEN_PYCOMMENT,
	TOKEN_LINECOMMENT,
	TOKEN_BLOCKCOMMENT,

	TOKEN_IDENT,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_LCBRACKET,
	TOKEN_RCBRACKET,
	TOKEN_COMMA,
	TOKEN_SEMICOLON,
	TOKEN_RANGE,
	TOKEN_ISTYPEOF,

    TOKEN_OP,  //EQUALS to THE NUMBER OF LANGUAGE KEYWORDS
};

enum op_code {
    OP_NULL,
    
 	OP_DOT,

	OP_BITNOT,
	OP_BITOR,

	OP_BITEXOR,
	OP_BAND,
	OP_BSL,
	OP_BSR,

	OP_STAR,
	OP_DIVISION,
	OP_MODULUS,
	OP_PLUS,
	OP_MINUS,
	
	OP_ASSIGN,

	OP_AT,

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
#define MAX_NONTERMS 2048
#define MAX_GRAMMAR_SYMBOLS 2048//(TERMINAL_COUNT + MAX_NONTERMS)


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

void token_init();

void token_deinit();

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type);

struct token_patterns get_token_patterns();

#define is_open_group(tp) (tp == TOKEN_LPAREN || tp == TOKEN_LBRACKET || tp == TOKEN_LCBRACKET)
#define is_close_group(tp) (tp == TOKEN_RPAREN || tp == TOKEN_RBRACKET || tp == TOKEN_RCBRACKET)
#define is_match_open(open, close) ((open == TOKEN_LPAREN && close == TOKEN_RPAREN) || (open == TOKEN_LBRACKET && close == TOKEN_RBRACKET) || (open == TOKEN_LCBRACKET && close == TOKEN_RCBRACKET))

#define is_comment_token(tp) (tp == TOKEN_PYCOMMENT || tp == TOKEN_LINECOMMENT || tp == TOKEN_BLOCKCOMMENT)
#define is_linecomment_token(tp) (tp == TOKEN_PYCOMMENT || tp == TOKEN_LINECOMMENT)

void tok_clean(struct token *tok);


#ifdef __cplusplus
}
#endif

#endif
