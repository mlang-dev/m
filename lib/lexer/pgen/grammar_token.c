/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * token.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/pgen/grammar_token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define TOKEN(tok_name, name, pattern, class_name) {name, pattern, TOKEN_##tok_name, 0, class_name, 0, 0}
#define OP(op_name, name, pattern) {name, pattern, TOKEN_OP, OP_##op_name, "operator", 0, 0}

struct token_pattern _token_patterns[TERMINAL_COUNT] = {
    TOKEN(NULL, "NULL",  0, 0),    // 1
    TOKEN(EOF, "EOF",  0, 0),     // 1
    TOKEN(EPSILON, "EPSILON", 0, 0),

    TOKEN(INDENT, "INDENT", 0, 0),
    TOKEN(DEDENT, "DEDENT", 0, 0),
    TOKEN(LITERAL_CHAR, "LITERAL_CHAR", 0, "string"),
    TOKEN(LITERAL_STRING, "LITERAL_STRING", 0, "string"),
    TOKEN(NEWLINE, "NEWLINE", "\n", 0),
    TOKEN(LITERAL_INT, "LITERAL_INT", "[0-9]+|0x[0-9a-fA-F]+", "number"),
    TOKEN(LITERAL_FLOAT, "LITERAL_FLOAT", "([0-9]*.)?[0-9]+", "number"),

    TOKEN(PYCOMMENT, "#", "#", "pycomment"),
    TOKEN(LINECOMMENT, "//", "//", "comment"),
    TOKEN(BLOCKCOMMENT, "/*", "/\\*", "block-comment"),

    TOKEN(IDENT, "IDENT", "[_a-zA-Z][_a-zA-Z0-9]*", 0), 

    //operators
    TOKEN(LPAREN, "(", "\\(", "keyword"),
    TOKEN(RPAREN, ")", "\\)", "keyword"),
    TOKEN(LBRACKET, "[", "\\[", "keyword"),
    TOKEN(RBRACKET, "]", "\\]", "keyword"),
    TOKEN(LCBRACKET, "{", "{", "keyword"), // 25
    TOKEN(RCBRACKET, "}", "}", "keyword"),

    TOKEN(COMMA, ",", ",", "keyword"),

    /*operator separator*/
    TOKEN(OP, "OP", 0, "operator"),
    OP(BITOR, "|", "\\|"),
    OP(ASSIGN, "=", "="),
};

void token_init()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &_token_patterns[i];
        if(tp->token_name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->token_name){
            tp->symbol_name = to_symbol(tp->token_name);
        }
    }
}

void token_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &_token_patterns[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
}

void tok_clean(struct token *tok)
{
    if(tok->token_type == TOKEN_LITERAL_STRING && tok->str_val){
        FREE((void*)tok->str_val);
        tok->str_val = 0;
    }
}

struct token_patterns get_token_patterns()
{
    struct token_patterns tps = { _token_patterns, TERMINAL_COUNT };
    return tps;
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &_token_patterns[(int)token_type];
}
