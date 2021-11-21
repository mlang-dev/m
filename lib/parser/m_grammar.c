/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/m_grammar.h"
#include "clib/util.h"


const char *keyword_symbols[] = {
    "import",
    "extern",
    "type",
    "if",
    "else",
    "then",
    "in",
    "for",
    "unary",
    "binary",
    "..",
    "...",
    "true",
    "false",
    "||",
    "&&",
    "!",
    "%",
    "&",
    "^",
    "*",
    "/",
    "+",
    "-",
    "<",
    "=",
    ">",
    "<=",
    ">=",
    "==",
    "!=",
    "?",
    "@",
    "|",
    ":",
    "(",
    ")",
    "[",
    "]"
};

int keyword_count = ARRAY_SIZE(keyword_symbols);

const char *get_m_grammar()
{
    const char *m_grammar = 
        "program    = program stmt      { list 0 1 }"
        "           | stmt              { 0 }"
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