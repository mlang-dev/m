#include "lexer/token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name) {0, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name) {#tok_name, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name) {keyword, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}
#define NAME_KEYWORD_PATTERN(name, keyword, tok_name, op_name) {name, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}

struct token_pattern g_nonterms[MAX_NONTERMS];
u16 g_nonterm_count;

struct hashtable token_patterns_by_symbol;

void token_init()
{
    terminal_init();
    hashtable_init(&token_patterns_by_symbol);
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->name){
            hashtable_set_p(&token_patterns_by_symbol, tp->symbol_name, tp);
        }
    }
    g_nonterm_count = 0;
}

void token_deinit()
{
    hashtable_deinit(&token_patterns_by_symbol);
    terminal_deinit();
}

struct token_pattern *get_token_pattern_by_symbol(symbol symbol)
{
    return (struct token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}

//the symbol could be terminal or non-terminal symbol in grammar
u16 _get_symbol_index(symbol symbol)
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

u16 get_token_index(enum token_type token_type, enum op_code opcode)
{
    symbol symbol = get_symbol_by_token_opcode(token_type, opcode);
    return _get_symbol_index(symbol);
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
