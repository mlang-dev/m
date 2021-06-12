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

struct grammar *create_meta_grammar();
void free_meta_grammar(struct grammar *meta_grammar);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_META_GRAMMAR_H__
