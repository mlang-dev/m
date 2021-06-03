/*
 * mgrammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for meta grammar: the grammar of grammar
 */
#ifndef __MLANG_META_GRAMMAR_H__
#define __MLANG_META_GRAMMAR_H__
#ifdef __cplusplus
extern "C" {
#endif

const char *meta_grammar = R"(

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
                | NUM               {}
                | STR               {}

)";

#ifdef __cplusplus
}
#endif

#endif //__MLANG_META_GRAMMAR_H__
