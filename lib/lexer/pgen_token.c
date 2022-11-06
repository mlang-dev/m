/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * defines token functions used in pgen.
 *
 */
#include "lexer/pgen_token.h"
#include "clib/hashtable.h"
#include "lexer/terminal.h"
#include <assert.h>

struct token_pattern g_nonterms[MAX_NONTERMS];
u16 g_nonterm_count;

struct hashtable token_patterns_by_symbol;

void pgen_token_init()
{
    hashtable_init(&token_patterns_by_symbol);
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->name){
            tp->symbol_name = to_symbol(tp->name);
            hashtable_set_p(&token_patterns_by_symbol, tp->symbol_name, tp);
        }
    }
    g_nonterm_count = 0;
}

void pgen_token_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
    hashtable_deinit(&token_patterns_by_symbol);
}

struct token_pattern *_get_token_pattern_by_symbol(symbol symbol)
{
    return (struct token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}

//the symbol could be terminal or non-terminal symbol in grammar
u16 get_symbol_index(symbol symbol)
{
    struct token_pattern *tp = _get_token_pattern_by_symbol(symbol);
    if(tp == 0){
        printf("unknown symbol: %s\n", string_get(symbol));
        assert(false);
    }
    
    if(tp->token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)tp->opcode;
    return (u16)tp->token_type;
}


u16 register_grammar_nonterm(symbol symbol)
{
    struct token_pattern *tp = _get_token_pattern_by_symbol(symbol);
    if(tp){
        printf("already found the grammar nonterm symbol: %s\n", string_get(symbol));
        assert(false);
    }
    //add nonterm grammar symbol, TODO: need to check existence of symbol, don't add if already existed
    struct token_pattern tpn;
    u16 nonterm = (u16)g_nonterm_count + (u16)TERMINAL_COUNT;
    tpn.token_type = nonterm;
    tpn.symbol_name = symbol;
    tpn.name = string_get(tpn.symbol_name);
    tpn.opcode = 0;
    tpn.pattern = 0;
    tpn.re = 0; 
    g_nonterms[g_nonterm_count] = tpn;
    hashtable_set_p(&token_patterns_by_symbol, tpn.symbol_name, &g_nonterms[g_nonterm_count]);
    g_nonterm_count++;  
    return nonterm;
}

u16 get_symbol_count()
{
    return g_nonterm_count + (u16)TERMINAL_COUNT;
}

symbol get_symbol_by_index(u16 symbol_index)
{
    struct token_pattern *tp;
    if (symbol_index < TERMINAL_COUNT){
        tp = &g_token_patterns[symbol_index];
    }else{
        symbol_index -= TERMINAL_COUNT;
        tp = &g_nonterms[symbol_index];
    }
    return tp->symbol_name;
}
