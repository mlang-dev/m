/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * terminal.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/terminal.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name) {0, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name) {#tok_name, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name) {keyword, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}
#define NAME_KEYWORD_PATTERN(name, keyword, tok_name, op_name) {name, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}

struct token_pattern g_token_patterns[TERMINAL_COUNT] = {
    TOKEN_PATTERN(0, ERROR, NULL), // 1
    TOKEN_PATTERN(0, EOF, NULL), // 1
    TOKEN_PATTERN(0, EPSILON, NULL),
    TOKEN_PATTERN(0, INDENT, NULL),
    TOKEN_PATTERN(0, DEDENT, NULL),
    TOKEN_PATTERN("\n", NEWLINE, NULL),
    TOKEN_PATTERN("[0-9]+|0x[0-9a-fA-F]+", INT, NULL), // 5
    TOKEN_PATTERN("([0-9]*.)?[0-9]+", DOUBLE, NULL),
    TOKEN_PATTERN("([0-9]*.)?[0-9]+ \\+ ([0-9]*.)?[0-9]*i", COMPLEX, NULL),
    TOKEN_PATTERN(0, CHAR, NULL),
    TOKEN_PATTERN(0, STRING, NULL),

    KEYWORD_PATTERN("from", FROM, NULL), // 10
    KEYWORD_PATTERN("import", IMPORT, NULL),
    KEYWORD_PATTERN("memory", MEMORY, NULL),
    KEYWORD_PATTERN("extern", EXTERN, NULL), // 10
    KEYWORD_PATTERN("enum", ENUM, NULL),
    KEYWORD_PATTERN("struct", STRUCT, NULL),
    KEYWORD_PATTERN("union", UNION, NULL),
    KEYWORD_PATTERN("type", TYPE, NULL),
    KEYWORD_PATTERN("let", LET, NULL),
    KEYWORD_PATTERN("fun", FUN, NULL),
    KEYWORD_PATTERN("->", MAPTO, NULL),
    KEYWORD_PATTERN("if", IF, NULL),
    KEYWORD_PATTERN("then", THEN, NULL),
    KEYWORD_PATTERN("else", ELSE, NULL), // 15

    KEYWORD_PATTERN("true", TRUE, NULL),
    KEYWORD_PATTERN("false", FALSE, NULL),
    KEYWORD_PATTERN("in", IN, NULL),
    KEYWORD_PATTERN("for", FOR, NULL),
    KEYWORD_PATTERN("while", WHILE, NULL),
    KEYWORD_PATTERN("break", BREAK, NULL),
    KEYWORD_PATTERN("continue", CONTINUE, NULL),

    TOKEN_PATTERN("[_a-zA-Z][_a-zA-Z0-9]*", IDENT, NULL), //

    NAME_KEYWORD_PATTERN("(", "\\(", LPAREN, NULL),
    NAME_KEYWORD_PATTERN(")", "\\)", RPAREN, NULL),
    NAME_KEYWORD_PATTERN("()", "\\(\\)", UNIT, NULL),
    NAME_KEYWORD_PATTERN("[", "\\[", LBRACKET, NULL),
    NAME_KEYWORD_PATTERN("]", "\\]", RBRACKET, NULL),
    KEYWORD_PATTERN("{", LCBRACKET, NULL), // 25
    KEYWORD_PATTERN("}", RCBRACKET, NULL),

    KEYWORD_PATTERN(",", COMMA, NULL),
    KEYWORD_PATTERN(";", SEMICOLON, NULL),


    NAME_KEYWORD_PATTERN("..", "\\.\\.", RANGE, NULL),
    NAME_KEYWORD_PATTERN("...", "\\.\\.\\.", VARIADIC, NULL), // 30
    KEYWORD_PATTERN(":", ISTYPEOF, NULL),

    /*reserved keywords*/
    KEYWORD_PATTERN("do", DO, NULL),
    KEYWORD_PATTERN("switch", SWITCH, NULL),
    KEYWORD_PATTERN("case", CASE, NULL),
    KEYWORD_PATTERN("default", DEFAULT, NULL),
    KEYWORD_PATTERN("return", RETURN, NULL),
    KEYWORD_PATTERN("yield", YIELD, NULL),
    KEYWORD_PATTERN("async", ASYNC, NULL),
    KEYWORD_PATTERN("await", AWAIT, NULL),
    KEYWORD_PATTERN("match", MATCH, NULL),
    KEYWORD_PATTERN("with", WITH, NULL),

    /*operators*/
    TOKEN_PATTERN(0, OP, NULL),
    NAME_KEYWORD_PATTERN(".", "\\.", OP, DOT), // literal dot
    NAME_KEYWORD_PATTERN("||", "\\|\\|", OP, OR),
    KEYWORD_PATTERN("&&", OP, AND), // 35
    KEYWORD_PATTERN("!", OP, NOT),

    KEYWORD_PATTERN("~", OP, BITNOT),
    NAME_KEYWORD_PATTERN("|", "\\|", OP, BITOR),
    KEYWORD_PATTERN("^", OP, BITEXOR),
    KEYWORD_PATTERN("&", OP, BAND), //or reference
    KEYWORD_PATTERN("<<", OP, BSL),
    KEYWORD_PATTERN(">>", OP, BSR),

    // KEYWORD_PATTERN("^", OP, EXPO),
    NAME_KEYWORD_PATTERN("|/", "\\|/", OP, SQRT),
    NAME_KEYWORD_PATTERN("**", "\\*\\*", OP, POW), // 40
    NAME_KEYWORD_PATTERN("*", "\\*", OP, STAR), // 40
    KEYWORD_PATTERN("/", OP, DIVISION),
    KEYWORD_PATTERN("%", OP, MODULUS),
    NAME_KEYWORD_PATTERN("+", "\\+", OP, PLUS),
    KEYWORD_PATTERN("-", OP, MINUS),

    KEYWORD_PATTERN("<", OP, LT), // 45
    KEYWORD_PATTERN("<=", OP, LE),
    KEYWORD_PATTERN("==", OP, EQ),
    KEYWORD_PATTERN(">", OP, GT),
    KEYWORD_PATTERN(">=", OP, GE),
    KEYWORD_PATTERN("!=", OP, NE),
    NAME_KEYWORD_PATTERN("?", "\\?", OP, COND), // 40

    KEYWORD_PATTERN("=", OP, ASSIGN),
    NAME_KEYWORD_PATTERN("*=", "\\*=", OP, MUL_ASSN),
    KEYWORD_PATTERN("/=", OP, DIV_ASSN),
    KEYWORD_PATTERN("%=", OP, MOD_ASSN),
    NAME_KEYWORD_PATTERN("+=", "\\+=", OP, ADD_ASSN),
    KEYWORD_PATTERN("-=", OP, SUB_ASSN),
    KEYWORD_PATTERN("<<=", OP, LEFT_ASSN),
    KEYWORD_PATTERN(">>=", OP, RIGHT_ASSN),
    KEYWORD_PATTERN("&=", OP, AND_ASSN),
    KEYWORD_PATTERN("^=", OP, XOR_ASSN),
    NAME_KEYWORD_PATTERN("|=", "\\|=", OP, OR_ASSN),

    NAME_KEYWORD_PATTERN("++", "\\+\\+", OP, INC),
    KEYWORD_PATTERN("--", OP, DEC),
};


const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};


void terminal_init()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->name){
            tp->symbol_name = to_symbol(tp->name);
        }
    }
}

void terminal_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
}


struct token_patterns get_token_patterns()
{
    struct token_patterns tps = { g_token_patterns, TERMINAL_COUNT };
    return tps;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < OP_TOTAL);
    return get_token_pattern_by_opcode(opcode)->name;
}

symbol get_terminal_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode)
{
    if (token_type == TOKEN_OP){
        assert(opcode >= 0 && opcode < OP_TOTAL);
        return get_token_pattern_by_opcode(opcode)->symbol_name;
    }else{
        return get_token_pattern_by_token_type(token_type)->symbol_name;
    }
}

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < TERMINAL_COUNT);
    return &g_token_patterns[(int)TOKEN_OP + (int)opcode];
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &g_token_patterns[(int)token_type];
}

u16 get_terminal_token_index(enum token_type token_type, enum op_code opcode)
{
    if(token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)opcode;
    else 
        return (u16)token_type;
}

bool is_terminal(u16 symbol_index)
{
    return symbol_index < TERMINAL_COUNT;
}

bool is_relational_op(enum op_code opcode)
{
    return opcode >= OP_LT && opcode <= OP_NE;
}

enum op_code get_op_code_from_assign_op(enum op_code assign_op)
{
    switch (assign_op)
    {
    case OP_MUL_ASSN:
        return OP_STAR;
    case OP_DIV_ASSN:
        return OP_DIVISION;
    case OP_MOD_ASSN:
        return OP_MODULUS;
    case OP_ADD_ASSN:
        return OP_PLUS;
    case OP_SUB_ASSN:
        return OP_MINUS;
    case OP_LEFT_ASSN:
        return OP_BSL;
    case OP_RIGHT_ASSN:
        return OP_RIGHT_ASSN;
    case OP_AND_ASSN:
        return OP_BAND;
    case OP_XOR_ASSN:
        return OP_BITEXOR;
    case OP_OR_ASSN:
        return OP_BITOR;
    default:
        assert(false);
        return OP_NULL;
    }
}
