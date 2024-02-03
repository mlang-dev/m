/*
 * lang_token.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for token definition of language used in parser generator
 */

#ifndef __MLANG_PGEN_TOKEN_H__
#define __MLANG_PGEN_TOKEN_H__

#include "lexer/source_location.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/typedef.h"
#include "pgen/grammar.h"

#ifdef __cplusplus
extern "C" {
#endif


struct lang_token_pattern{
    const char *token_name;       //c string name
    const char *pattern;    //pattern
    u16 token_type;
    u16 opcode; 

    const char *style_class_name; //ui style class name, used for syntax highlight
    symbol symbol_name;     //symbol name
    struct re *re;          //regex for the pattern
};

struct lang_token_pattern *create_lang_token_pattern(u16 token_type, const char *name, const char *pattern, const char *class_name);
struct lang_token_pattern *create_lang_op_pattern(u16 token_op, u16 op_type, const char *name, const char *pattern);

void lang_token_init();
void lang_token_deinit();

/*get symbol index, token or nonterm*/
u16 get_lang_symbol_index(symbol symbol);
symbol get_lang_symbol_by_index(u16 symbol_index);

/*get total symbol count including terminal tokens and nonterm symbols*/
u16 get_lang_symbol_count();

/*register nonterm symbol for grammar*/
u16 register_lang_grammar_nonterm(symbol symbol);


#ifdef __cplusplus
}
#endif

#endif
