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
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"
#include "lexer/terminal.h"

#ifdef __cplusplus
extern "C" {
#endif

void pgen_token_init();

void pgen_token_deinit();


struct token_pattern *get_token_pattern_by_symbol(symbol symbol);


/*get total symbol count including terminal tokens and nonterm symbols*/
u16 pgen_get_symbol_count();

/*get symbol index, token or nonterm*/
u16 pgen_get_symbol_index(symbol symbol);


/*register nonterm symbol for grammar in parser generator pgen*/
u16 pgen_register_grammar_nonterm(symbol symbol);

symbol pgen_get_symbol_by_index(u16 symbol_index);

symbol get_symbol_by_index(u16 symbol_index);


#ifdef __cplusplus
}
#endif

#endif
