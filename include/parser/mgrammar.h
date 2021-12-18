/*
 * mgrammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for grammar of mlang
 */
#ifndef __MLANG_MGRAMMAR_H__
#define __MLANG_MGRAMMAR_H__
#ifdef __cplusplus
extern "C" {
#endif

const char *m_grammar = R"(

sum         ::= sum '+' term        {}
                | sum '-' term      {}
                | term              {}
term        ::= term '*' factor     {}
                | term '/' factor   {}
                | term '%' factor   {}
                | factor            {}
factor      ::= '+' factor          {}
                | '-' factor        {}
                | power             {}
power       ::= primary '^' factor  {}
                | primary
primary     ::= primary '.' ID       {}
                | atom              {}
atom        ::= ID                  {}
                | INT               {}
                | STR               {}

)";

#ifdef __cplusplus
}
#endif

#endif
