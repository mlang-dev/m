/*
 * mgrammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for meta grammar: the grammar of grammar
 */
#ifndef __MLANG_META_GRAMMAR_H__
#define __MLANG_META_GRAMMAR_H__

#include "parser/grammar.h"

#ifdef __cplusplus
extern "C" {
#endif

// const char *meta_grammar_text = R"(
// grammar = rules EOF
// rules   = rules rule | rule
// rule    = IDENT "=" exprs NEWLINE
// exprs   = exprs "|" expr | expr
// expr    = expr atom | atom
// atom    = IDENT | STRING
// )";

struct grammar *create_meta_grammar();
void free_meta_grammar(struct grammar *meta_grammar);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_META_GRAMMAR_H__
