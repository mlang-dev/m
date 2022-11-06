/*
 * pgen_token.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for token definition used in parser generator
 */

#ifndef __MLANG_PGEN_TOKEN_H__
#define __MLANG_PGEN_TOKEN_H__

#include "lexer/source_location.h"
#include "lexer/terminal.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

void pgen_token_init();

void pgen_token_deinit();

/*get symbol index, token or nonterm*/
u16 get_symbol_index(symbol symbol);
symbol get_symbol_by_index(u16 symbol_index);

/*get total symbol count including terminal tokens and nonterm symbols*/
u16 get_symbol_count();

/*register nonterm symbol for grammar*/
u16 register_grammar_nonterm(symbol symbol);


#ifdef __cplusplus
}
#endif

#endif
