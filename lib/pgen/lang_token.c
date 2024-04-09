/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * defines token functions used in pgen.
 *
 */
#include "pgen/lang_token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define MAX_LANG_NONTERMS 2048

struct lang_token_pattern g_nonterms[MAX_LANG_NONTERMS];
u16 g_nonterm_count;
u16 g_terminal_count;

u16 g_token_op;
struct hashtable token_patterns_by_symbol;
struct array lang_token_patterns;

void free_lang_token_pattern(void *p)
{
    struct lang_token_pattern *tp = p;
    if(tp->re){
        regex_free(tp->re);
        tp->re = 0;
    }
    free(tp);
}

void fill_in_symbol_re(struct lang_token_pattern *tp)
{
    if(tp->token_name&&tp->pattern&&!tp->re){
        tp->re = regex_new(tp->pattern);
        assert(tp->re);
    }else{
        tp->re = 0;
    }
    if(tp->token_name){
        tp->symbol_name = to_symbol(tp->token_name);
        hashtable_set_p(&token_patterns_by_symbol, tp->symbol_name, tp);
    }

}
struct lang_token_pattern *create_lang_token_pattern(u16 token_type, const char *name, const char *pattern, const char *class_name)
{
    struct lang_token_pattern *tp;
    MALLOC(tp, sizeof(*tp));
    tp->token_name = name;
    tp->pattern = pattern;
    tp->token_type = token_type;
    tp->opcode = 0;
    tp->style_class_name = class_name;
    fill_in_symbol_re(tp);
    array_push_ptr(&lang_token_patterns, tp);
    g_terminal_count ++;
    return tp;
} 

struct lang_token_pattern *create_lang_op_pattern(u16 token_op, u16 op_type, const char *name, const char *pattern)
{
    struct lang_token_pattern *tp;
    MALLOC(tp, sizeof(*tp));
    tp->token_name = name;
    tp->pattern = pattern;
    tp->token_type = token_op;
    tp->opcode = op_type;
    tp->style_class_name = "operator";
    fill_in_symbol_re(tp);
    array_push_ptr(&lang_token_patterns, tp);
    g_token_op = token_op; //hacky way of set g_token_op to always point to last value of token
    g_terminal_count ++;
    return tp;
}

void lang_token_init(void)
{
    hashtable_init(&token_patterns_by_symbol);
    array_init_free(&lang_token_patterns, sizeof(struct lang_token_pattern *), free_lang_token_pattern);
    g_nonterm_count = 0;
    g_terminal_count = 0;
    g_token_op = 0;
}

void lang_token_deinit(void)
{
    array_deinit(&lang_token_patterns);
    hashtable_deinit(&token_patterns_by_symbol);
}

struct lang_token_pattern *_get_lang_token_pattern_by_symbol(symbol symbol)
{
    return (struct lang_token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}

//the symbol could be terminal or non-terminal symbol in grammar
u16 get_lang_symbol_index(symbol symbol)
{
    struct lang_token_pattern *tp = _get_lang_token_pattern_by_symbol(symbol);
    if(tp == 0){
        printf("unknown symbol: %s\n", string_get(symbol));
        assert(false);
    }
    u16 token_type = tp->token_type == g_token_op ? g_token_op + tp->opcode : tp->token_type;
    return token_type;
}

u16 register_lang_grammar_nonterm(symbol symbol)
{
    struct lang_token_pattern *tp = _get_lang_token_pattern_by_symbol(symbol);
    if(tp){
        printf("already found the grammar nonterm symbol: %s\n", string_get(symbol));
        assert(false);
    }
    //add nonterm grammar symbol, TODO: need to check existence of symbol, don't add if already existed
    struct lang_token_pattern tpn;
    u16 nonterm = g_nonterm_count + g_terminal_count;
    tpn.token_type = nonterm;
    tpn.symbol_name = symbol;
    tpn.token_name = string_get(tpn.symbol_name);
    tpn.opcode = 0;
    tpn.pattern = 0;
    tpn.re = 0; 
    g_nonterms[g_nonterm_count] = tpn;
    hashtable_set_p(&token_patterns_by_symbol, tpn.symbol_name, &g_nonterms[g_nonterm_count]);
    g_nonterm_count++;  
    return nonterm;
}

u16 get_lang_symbol_count(void)
{
    return g_nonterm_count + g_terminal_count;
}

symbol get_lang_symbol_by_index(u16 symbol_index)
{
    struct lang_token_pattern *tp;
    if (symbol_index < g_terminal_count){
        tp = array_get_ptr(&lang_token_patterns, symbol_index);
    }else{
        symbol_index -= g_terminal_count;
        tp = &g_nonterms[symbol_index];
    }
    return tp->symbol_name;
}
