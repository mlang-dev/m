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
    const char *m_grammar = "start      = block             { 0 }\n"
                            "block      = block stmt        { block 0 1 }\n"
                            "           | stmt              { block 0 }\n"
                            "stmt       = IDENT '(' ')' '=' expr  { func 0 4 }\n"
                            "           | expr              { 0 }\n"
                            "expr       = expr [+-] term    { binop 0 1 2 }\n"
                            "           | term              { 0 }\n"
                            "term       = term [*/%] factor { binop 0 1 2 }\n"
                            "           | factor            { 0 }\n"
                            "factor     = '(' expr ')'      { 1 }\n"
                            "           | power             { 0 }\n"
                            "power      = INT '^' factor    { binop 0 1 2 }\n"
                            "           | INT               { 0 }";
    return m_grammar;
}

// const char *m_grammar = "start      = block             { 0 }\n"
//                         "block      = block stmt        { block 0 1 }\n"
//                         "           | stmt              { block 0 }\n"
//                         "stmt       = IDENT '(' ')' '=' expr  { func 0 4 }\n"
//                         "           | expr              { 0 }\n"
//                         "expr       = expr [+-] term    { binop 0 1 2 }\n"
//                         "           | term              { 0 }\n"
//                         "term       = term [*/%] factor { binop 0 1 2 }\n"
//                         "           | factor            { 0 }\n"
//                         "factor     = '(' expr ')'      { 1 }\n"
//                         "           | [+-] factor       { unop 0 1 }\n"
//                         "           | power             { 0 }\n"
//                         "power      = INT '^' factor    { binop 0 1 2 }\n"
//                         "           | INT               { 0 }";

// const char *grammar = "s  = e             {}\n"
//                         "e  = e '+' t       {binop 0 1 2}\n"
//                         "   | t             {}\n"
//                         "t  = t '*' f       {binop 0 1 2}\n"
//                         "   | f             {}\n"
//                         "f  = '(' e ')'     {1}\n"
//                         "   | INT           {}\n";
