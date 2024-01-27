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
	TOKEN_WILDCARD,
	TOKEN_FROM,
	TOKEN_MEMORY,
	TOKEN_EXTERN,
	TOKEN_NONE,
	TOKEN_TYPE_BOOL,
	TOKEN_TYPE_INT,
	TOKEN_TYPE_U8,
	TOKEN_TYPE_U16,
	TOKEN_TYPE_U32,
	TOKEN_TYPE_U64,
	TOKEN_TYPE_I8,
	TOKEN_TYPE_I16,
	TOKEN_TYPE_I32,
	TOKEN_TYPE_I64,
	TOKEN_TYPE_F32,
	TOKEN_TYPE_F64,
	TOKEN_TYPE_CHAR,
	TOKEN_TYPE_STRING,
	TOKEN_USE,
	TOKEN_TYPE,
	TOKEN_RESOURCE,
	TOKEN_FUN,
	TOKEN_STRUCT,
	TOKEN_RECORD,
	TOKEN_ENUM,
	TOKEN_FLAGS,
	TOKEN_VARIANT,
	TOKEN_UNION,
	TOKEN_OPTION,
	TOKEN_LIST,
	TOKEN_RESULT,
	TOKEN_AS,
	TOKEN_STATIC,
	TOKEN_INTERFACE,
	TOKEN_WORLD,
	TOKEN_TUPLE,
	TOKEN_FUTURE,
	TOKEN_STREAM,
	TOKEN_IMPORT,
	TOKEN_EXPORT,
	TOKEN_PACKAGE,
	TOKEN_INCLUDE,
	TOKEN_CLASS,
	TOKEN_BORROW,
	TOKEN_DEFAULT,
	TOKEN_DEF,
	TOKEN_LET,
	TOKEN_MUT,
	TOKEN_MAPTO,
	TOKEN_IF,
	TOKEN_ELIF,
	TOKEN_ELSE,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_IN,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_BREAK,
	TOKEN_CONTINUE,
	TOKEN_RETURN,
	TOKEN_CONSTRUCTOR,
	TOKEN_MATCH,
	TOKEN_WITH,
	TOKEN_WHEN,
	TOKEN_NEW,
	TOKEN_DEL,
	TOKEN_YIELD,
	TOKEN_ASYNC,
	TOKEN_AWAIT,
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
	TOKEN_VARIADIC,
	TOKEN_ISTYPEOF,

    TOKEN_OP,  //EQUALS to THE NUMBER OF LANGUAGE KEYWORDS + 3
};

enum op_code {
    OP_NULL,
    
 	OP_DOT,
	OP_OR,
	OP_AND,
	OP_NOT,
	OP_BITNOT,
	OP_BITOR,
	OP_BITEXOR,
	OP_BAND,
	OP_BSL,
	OP_BSR,
	OP_SQRT,
	OP_POW,
	OP_STAR,
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
	OP_COND,
	OP_ASSIGN,
	OP_MUL_ASSN,
	OP_DIV_ASSN,
	OP_MOD_ASSN,
	OP_ADD_ASSN,
	OP_SUB_ASSN,
	OP_LEFT_ASSN,
	OP_RIGHT_ASSN,
	OP_AND_ASSN,
	OP_XOR_ASSN,
	OP_OR_ASSN,
	OP_INC,
	OP_DEC,
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

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode);

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type);

struct token_patterns get_token_patterns();

symbol get_terminal_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode);

/*get index by token*/
u16 get_terminal_token_index(enum token_type token_type, enum op_code opcode);

const char *get_opcode(enum op_code opcode);

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
