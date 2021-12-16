/*
 * grammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for grammar of m
 */
#ifndef __MLANG_M_GRAMMAR_H__
#define __MLANG_M_GRAMMAR_H__

#include "lexer/token.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int keyword_count;

const char * get_m_grammar();

#ifdef __cplusplus
}
#endif

#endif
