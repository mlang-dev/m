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
#include "clib/array.h"
#include "clib/hashset.h"

#ifdef __cplusplus
extern "C" {
#endif

enum expr_item_type {
    EI_NONTERM = 0,
    EI_TOKEN_MATCH,  // like ID, NUM token
    EI_EXACT_MATCH,
    EI_IN_MATCH,
};

struct expr_item {
    symbol sym;
    enum expr_item_type ei_type;
    struct array members; // sets of members, when ei_type is in EI_IN_MATCH or EI_RANGE_MATCH
};

struct expr {
    struct array items;     //array of symbols (terminal or nonterminal)
    struct array actions;   //semantic actions
};

struct rule {
    int rule_no;
    symbol nonterm;     // nonterminal symbol
    struct array exprs; // array of expr
};

/// A grammar is a set of Rules. A rule is the form of A ::= e where A is a nonterminal symbol
/// and e is a expression consists of any terminal symbol, any nonterminal symbol or empty string
/// e.g. product = product [*/] factor
struct grammar {
    symbol start_symbol;    // nonterminal symbol name
    hashset keywords; // keywords are from grammar production rule: 1. string literal, 2 char set 3. might extend to some regex
    struct hashtable rule_map; /* hashtable of <symbol, rule> */
    struct array rules;        /* array of rule pointer */
};

struct grammar *grammar_new();
struct grammar *grammar_parse(const char *grammar_text);
void grammar_free(struct grammar *grammar);

bool expr_item_exists_symbol(struct expr_item *ei, symbol sym);

#ifdef __cplusplus
}
#endif

#endif
