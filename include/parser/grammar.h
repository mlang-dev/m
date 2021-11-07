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

enum atom_type {
    ATOM_NONTERM = 0,
    ATOM_TOKEN_MATCH,  // like ID, NUM token
    ATOM_EXACT_MATCH,
    ATOM_IN_MATCH,
    ATOM_REGEX_MATCH
};

struct atom {
    symbol sym;
    enum atom_type type;
};

struct expr {
    struct array atoms; //array of symbols (terminal or nonterminal)
};

struct rule {
    int rule_no;
    symbol nonterm; // nonterminal symbol
    struct array exprs; // array of expr
};

/// A grammar is a set of Rules. A rule is the form of A ::= e where A is a nonterminal symbol
/// and e is a expression consists of any terminal symbol, any nonterminal symbol or empty string
/// e.g. product = product [*/] factor
struct grammar {
    symbol start_symbol; // nonterminal symbol name
    struct hashtable rule_map; /* hashtable of <symbol, rule> */
    struct array rules;        /* array of rule pointer */
};

struct grammar *grammar_new();
struct grammar *grammar_parse(const char *grammar_text);
void grammar_free(struct grammar *grammar);

#ifdef __cplusplus
}
#endif

#endif
