/*
 * grammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for grammar of language
 */
#ifndef __MLANG_GRAMMAR_H__
#define __MLANG_GRAMMAR_H__

#include "clib/symbol.h"
#include "clib/symboltable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct expr {
    struct array tokens; //array of tokens
};

struct rule {
    symbol nonterm; // nonterminal symbol
    struct array exprs; // array of expr
};

/// A grammar is a set of Rules. A rule is the form of A = e where A is a nonterminal symbol
/// and e is a expression consists of any terminal symbol, any nonterminal symbol or empty string
/// e.g. product = product [*/] factor
struct grammar {
    symbol start_symbol; // nonterminal symbol name
    struct hashtable rules; /* hashtable of <symbol, rule> */
};

struct grammar *grammar_new(const char *grammar_text);
void grammar_free(struct grammar *grammar);

#ifdef __cplusplus
}
#endif

#endif
