/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "parser/meta_grammar.h"
#include <assert.h>

const char *grammar_symbols[] = {
    "::=",  // 
    "|",    // 
    "[",    // range start symbol
    "]",    // range end symbol
    "?",    // optional
    "*",    // zero or more repeats,
    "+",    // one or more repeats
    "{",    // semantic action start
    "}"     // semantic action end
};

int grammar_symbols_count = ARRAY_SIZE(grammar_symbols);

void expr_add_symbol(struct expr *expr, symbol symbol, bool is_terminal)
{
    struct atom atom;
    atom.symbol = symbol;
    atom.is_terminal = is_terminal;
    array_push(&expr->atoms, &atom);
}

void expr_init(struct expr *expr)
{
    array_init(&expr->atoms, sizeof(struct atom));
}

void expr_deinit(struct expr *expr)
{
    array_deinit(&expr->atoms);
}

void rule_init(struct rule *rule)
{
    array_init(&rule->exprs, sizeof(struct expr));
}

struct expr* rule_add_expr(struct rule *rule)
{
    struct expr expr;
    expr_init(&expr);
    array_push(&rule->exprs, &expr);
    return (struct expr*)array_back(&rule->exprs);
}

void rule_deinit(struct rule *rule)
{
    for(size_t i = 0; i < array_size(&rule->exprs); i++){
        struct expr* expr = (struct expr*)array_get(&rule->exprs, i);
        expr_deinit(expr);
    }
    array_deinit(&rule->exprs);
}

struct grammar *grammar_new()
{
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashtable_init_with_value_size(&grammar->rule_map, sizeof(struct rule), (free_fun)rule_deinit);
    array_init(&grammar->rules, sizeof(struct rule*));
    grammar->start_symbol = 0;
    return grammar;
}

struct rule* grammar_add_rule(struct grammar* g, symbol nonterm, int rule_no)
{
    struct rule rule;
    rule.nonterm = nonterm;
    rule.rule_no = rule_no;
    rule_init(&rule);
    hashtable_set_p(&g->rule_map, nonterm, &rule);
    struct rule *r = (struct rule*)hashtable_get_p(&g->rule_map, nonterm);
    array_push(&g->rules, &r);
    return r;
}

struct grammar *grammar_parse(const char *grammar_text)
{
    symbol grmr_def = to_symbol("::=");
    symbol grmr_or = to_symbol("|");
    symbol grmr_act_start = to_symbol("{");
    symbol grmr_act_end = to_symbol("}");
    struct grammar *g = grammar_new();
    struct tokenizer *tokenizer = create_tokenizer_for_string(grammar_text, grammar_symbols, grammar_symbols_count);
    struct token *next_tok;
    struct token tok;
    symbol nonterm = 0;
    int rule_no = 0;
    struct rule* rule = 0;
    struct expr* expr = 0;
    next_tok = get_token(tokenizer);
    token_copy(&tok, next_tok);
    while(tok.token_type != TOKEN_EOF) {
        next_tok = get_token(tokenizer);
        if(tok.token_type == TOKEN_IDENT){
            if(next_tok->token_type==TOKEN_SYMBOL && next_tok->val.symbol_val == grmr_def){
                rule = grammar_add_rule(g, tok.val.symbol_val, rule_no++);
                expr = rule_add_expr(rule);
            }else{
                //nonterm symbol or terminal (all upper case)
                expr_add_symbol(expr, tok.val.symbol_val, is_upper(string_get(tok.val.symbol_val)));
            }
        }
        else if(tok.token_type == TOKEN_SYMBOL){
            if(tok.val.symbol_val == grmr_or){
            //new exp for the same rule (nonterm)
                assert(rule);
                expr = rule_add_expr(rule);
            }
        }
        else if(tok.token_type == TOKEN_STRING){
            //literal string is terminal symbol
            expr_add_symbol(expr, to_symbol(string_get(tok.val.str_val)), true);
        }
        else if(tok.token_type == TOKEN_CHAR){
            //literal string is terminal symbol
            char str[2];
            str[0] = tok.val.char_val;
            str[1] = 0;
            expr_add_symbol(expr, to_symbol(str), true);
        }
        token_copy(&tok, next_tok);
        //printf("got token: %s\n", token_type_strings[tok->token_type]);
    } 
    if(array_size(&g->rules)){
        g->start_symbol = (*(struct rule **)array_front(&g->rules))->nonterm;
    }
    destroy_tokenizer(tokenizer);
    return g;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rule_map);
    array_deinit(&grammar->rules);
    free(grammar);
}