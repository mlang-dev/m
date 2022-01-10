/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/m_grammar.h"
#include "clib/util.h"
#include <assert.h>


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
        "power      = INT '^' factor    { binop 0 1 2 }"
        "           | INT               { 0 }";
    return m_grammar;
}

const char *get_m_augmented_grammar()
{
    const char *m_grammar = 
        "start      = program           { 0 }"
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
        "power      = INT '^' factor    { binop 0 1 2 }"
        "           | INT               { 0 }";
    return m_grammar;
}
