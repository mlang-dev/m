#include "lexer/pgen_token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name) {0, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name) {#tok_name, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name) {keyword, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}
#define NAME_KEYWORD_PATTERN(name, keyword, tok_name, op_name) {name, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}

struct token_pattern g_nonterms[MAX_NONTERMS];
u16 g_nonterm_count;

struct hashtable token_patterns_by_symbol;

void pgen_token_init()
{
    hashtable_init(&token_patterns_by_symbol);
    g_nonterm_count = 0;
}

void pgen_token_deinit()
{
    hashtable_deinit(&token_patterns_by_symbol);
}

struct token_pattern *get_token_pattern_by_symbol(symbol symbol)
{
    return (struct token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}

u16 pgen_get_symbol_index(symbol symbol)
{
    struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
    if(tp == 0){
        printf("unknown symbol: %s\n", string_get(symbol));
        assert(false);
    }
    
    if(tp->token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)tp->opcode;
    return (u16)tp->token_type;
}

u16 pgen_register_grammar_nonterm(symbol symbol)
{
    struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
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

symbol pgen_get_symbol_by_index(u16 symbol_index)
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

u16 pgen_get_symbol_count()
{
    return g_nonterm_count + (u16)TERMINAL_COUNT;
}
