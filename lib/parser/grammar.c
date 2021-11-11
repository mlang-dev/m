/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "parser/tok.h"
#include "clib/util.h"
#include <assert.h>

void expr_item_init(struct expr_item *ei, symbol sym, enum expr_item_type type)
{
    ei->sym = sym;
    ei->ei_type = type;
    array_init(&ei->members, sizeof(symbol));
}

struct expr_item* expr_add_symbol(struct expr *expr, symbol sym, enum expr_item_type type)
{
    struct expr_item ei;
    expr_item_init(&ei, sym, type);
    array_push(&expr->items, &ei);
    return (struct expr_item*)array_back(&expr->items);
}

void expr_item_deinit(struct expr_item *ei)
{
    array_deinit(&ei->members);
}

bool expr_item_exists_symbol(struct expr_item *ei, symbol sym)
{
    for(size_t i = 0; i < array_size(&ei->members); i++){
        symbol x = *(symbol*)array_get(&ei->members, i);
        if(x == sym) return true;
    }
    return false;
}

void expr_init(struct expr *expr)
{
    expr->action.action = 0;
    expr->action.exp_item_index_count = 0;
    array_init(&expr->items, sizeof(struct expr_item));
}

void expr_deinit(struct expr *expr)
{
    for(size_t i = 0; i < array_size(&expr->items); i++){
        struct expr_item *ei = (struct expr_item *)array_get(&expr->items, i);
        expr_item_deinit(ei);
    }
    array_deinit(&expr->items);
}

void rule_init(struct rule *rule)
{
    array_init(&rule->exprs, sizeof(struct expr));
}

struct expr *rule_add_expr(struct rule *rule)
{
    struct expr expr;
    expr_init(&expr);
    array_push(&rule->exprs, &expr);
    return (struct expr *)array_back(&rule->exprs);
}

void rule_deinit(struct rule *rule)
{
    for (size_t i = 0; i < array_size(&rule->exprs); i++) {
        struct expr *expr = (struct expr *)array_get(&rule->exprs, i);
        expr_deinit(expr);
    }
    array_deinit(&rule->exprs);
}

struct grammar *grammar_new()
{
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashset_init(&grammar->keywords);
    hashtable_init_with_value_size(&grammar->rule_map, sizeof(struct rule), (free_fun)rule_deinit);
    array_init(&grammar->rules, sizeof(struct rule *));
    grammar->start_symbol = 0;
    return grammar;
}

struct rule *grammar_add_rule(struct grammar *g, symbol nonterm, int rule_no)
{
    struct rule rule;
    rule.nonterm = nonterm;
    rule.rule_no = rule_no;
    rule_init(&rule);
    hashtable_set_p(&g->rule_map, nonterm, &rule);
    struct rule *r = (struct rule *)hashtable_get_p(&g->rule_map, nonterm);
    array_push(&g->rules, &r);
    return r;
}

struct grammar *grammar_parse(const char *grammar_text)
{
    struct grammar *g = grammar_new();
    struct tok tok, next_tok;
    struct lexer lexer;
    lexer_init(&lexer, grammar_text);
    get_tok(&lexer, &tok);
    int rule_no = 0;
    struct rule *rule = 0;
    struct expr *expr = 0;
    symbol s = 0;
    string group;
    while (tok.tok_type) {
        get_tok(&lexer, &next_tok);
        if(tok.tok_type == lexer.IDENT_TOKEN){
            s = to_symbol2(&grammar_text[tok.start_pos], tok.end_pos - tok.start_pos);
            if(next_tok.char_type == '='){
                //nonterm
                rule = grammar_add_rule(g, s, rule_no++);
                expr = rule_add_expr(rule);
                get_tok(&lexer, &next_tok); //skip the '='
            }else if(expr){
                expr_add_symbol(expr, s, is_upper((string*)s) ? EI_TOKEN_MATCH : EI_NONTERM);
            }
        } else if (tok.tok_type == lexer.STRING_TOKEN || tok.tok_type == lexer.CHAR_TOKEN) {
            s = to_symbol2(&grammar_text[tok.start_pos+1], tok.end_pos - tok.start_pos - 2); //stripping off two single quote
            expr_add_symbol(expr, s, EI_EXACT_MATCH);
            hashset_set2(&g->keywords, string_get(s), string_size(s));
        }
        switch(tok.char_type){
            case '|':
                expr = rule_add_expr(rule);
                break;
            case '[': //regex
                string_init_chars2(&group, "", 0);
                while(next_tok.char_type != ']'){
                    const char *p = &grammar_text[next_tok.start_pos];
                    for(int i = 0; i < next_tok.end_pos - next_tok.start_pos; i ++){
                        hashset_set2(&g->keywords, p + i, 1);
                    }
                    string_add_chars2(&group, p, next_tok.end_pos - next_tok.start_pos);
                    get_tok(&lexer, &next_tok);
                }
                struct expr_item *ei = expr_add_symbol(expr, string_2_symbol2(&group), EI_IN_MATCH);
                for (size_t i = 0; i < string_size(&group); i++) {
                    symbol s = to_symbol2(string_get(&group) + i, 1);
                    array_push(&ei->members, &s);
                }
                get_tok(&lexer, &next_tok); // skip ']'
                break;
            case '{':
                while(next_tok.char_type != '}'){
                    //next tok is action
                    if(next_tok.tok_type == lexer.IDENT_TOKEN){
                        assert(expr->action.action == 0);
                        expr->action.action  = to_symbol2(&grammar_text[next_tok.start_pos], next_tok.end_pos - next_tok.start_pos);
                    }
                    else if(next_tok.tok_type == lexer.NUM_TOKEN){
                        expr->action.exp_item_index[expr->action.exp_item_index_count++] = grammar_text[next_tok.start_pos] - '0'; 
                    }
                    else
                        assert(false);
                    get_tok(&lexer, &next_tok);//'}'
                }
                get_tok(&lexer, &next_tok); //skip '}
                break;  
        }
        tok = next_tok;
        next_tok.tok_type = 0;
    }
    if (array_size(&g->rules)) {
        g->start_symbol = (*(struct rule **)array_front(&g->rules))->nonterm;
    }
    return g;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rule_map);
    array_deinit(&grammar->rules);
    hashset_deinit(&grammar->keywords);
    free(grammar);
}