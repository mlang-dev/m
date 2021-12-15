/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/m_grammar.h"
#include "lexer/m_lexer.h"
#include "clib/util.h"
#include <assert.h>

struct keyword_token keyword_tokens[] = {
    {"", TOKEN_NULL},
    {"\n", TOKEN_NEWLINE},
    {"\0", TOKEN_EOF},
    {"import", TOKEN_IMPORT},
    {"extern", TOKEN_EXTERN},
    {"type", TOKEN_TYPE},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"then", TOKEN_THEN},
    {"in", TOKEN_IN},
    {"for", TOKEN_FOR},
    {"unary", TOKEN_SYMBOL},
    {"binary", TOKEN_SYMBOL},
    {".", TOKEN_DOT},
    {"..", TOKEN_RANGE},
    {"...", TOKEN_SYMBOL},
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    {"&", TOKEN_SYMBOL},
    {"=", TOKEN_ASSIGN},
    {"?", TOKEN_SYMBOL},
    {"@", TOKEN_SYMBOL},
    {"|", TOKEN_SYMBOL},
    {":", TOKEN_SYMBOL},
    {"(", TOKEN_SYMBOL},
    {")", TOKEN_SYMBOL},
    {"[", TOKEN_SYMBOL},
    {"]", TOKEN_SYMBOL},

    {"||", OP_OR},
    {"&&", OP_AND},
    {"!", OP_NOT},
    {"^", OP_EXPO},
    {"*", OP_TIMES},
    {"/", OP_DIVISION},
    {"%", OP_MODULUS},
    {"+", OP_PLUS},
    {"-", OP_MINUS},
 
    {"<",  OP_LT},
    {">",  OP_GT},
    {"<=", OP_LE},
    {">=", OP_GE},
    {"==", OP_EQ},
    {"!=", OP_NE},
};


int keyword_count = ARRAY_SIZE(keyword_tokens);

const char * get_opcode(int token_or_opcode)
{
    static const char **token_opcodes = 0;
    if(!token_opcodes){
        CALLOC(token_opcodes, END_TOKENS, sizeof(const char *));
        for (int i = 0; i < keyword_count; i++){
            struct keyword_token kw_tt = keyword_tokens[i];
            assert((int)kw_tt.token_type < keyword_count);
            token_opcodes[kw_tt.token_type] = kw_tt.keyword; //token type includes opcodes
        }
    }
    return token_opcodes[token_or_opcode];
}

const char *get_m_grammar()
{
    const char *m_grammar = 
        "program    = program stmt      { prog 0 1 }"
        "           | stmt              { prog 0 }"
        "stmt       = IDENT '(' ')' '=' expr  { func 0 4 }"
        "           | expr              { 0 }"
        "expr       = expr [+-] term    { binop 0 1 2 }"
        "           | term              { 0 }"
        "term       = term [*/%] factor { binop 0 1 2 }"
        "           | factor            { 0 }"
        "factor     = '(' expr ')'      { 1 }"
        "           | [+-] factor       { unop 0 1 }"
        "           | power             { 0 }"
        "power      = NUM '^' factor    { binop 0 1 2 }"
        "           | NUM               { 0 }";
    return m_grammar;
}
