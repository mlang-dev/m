/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
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

void expr_init(struct expr *expr)
{
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
    const char *p = grammar_text;
    struct grammar *g = grammar_new();
    int rule_no = 0;
    struct rule *rule = 0;
    struct expr *expr = 0;
    char term[256];
    int term_char_no = 0;
    symbol s = 0;
    while (*p) {
        switch(*p){
            default:
                term[term_char_no++] = *p++;
                break;
            case '#':
                while(*p++ != '\n');
                break;
            case '=':
                //take 
                p++;
                break;
            case '\r':
            case '\n':
            case ' ':
                if (term_char_no) {
                    s = to_symbol2(term, term_char_no);
                    term_char_no = 0;
                }
                while(*p == '\r' || *p == '\n' || *p == ' ') p++;
                if(*p == '='){ //next char is definition
                    assert(term_char_no == 0);
                    rule = grammar_add_rule(g, s, rule_no++);
                    expr = rule_add_expr(rule);
                    s = 0;
                    p++;
                }else if(expr && s){
                    expr_add_symbol(expr, s, is_upper(term, sizeof(term)) ? EI_TOKEN_MATCH : EI_NONTERM);
                    s = 0;
                }
                break;
            case '|':
                assert(rule);
                expr = rule_add_expr(rule);
                p++;
                break;
            case '\'':
                assert(term_char_no == 0);
                while(*++p != '\''){
                    term[term_char_no++] = *p;
                }
                expr_add_symbol(expr, to_symbol2(term, term_char_no), EI_EXACT_MATCH);
                hashset_set2(&g->keywords, term, term_char_no);
                term_char_no = 0;
                p++;
                break;
            case '[': //regex
                assert(term_char_no == 0);
                while (*++p != ']') {
                    term[term_char_no++] = *p;
                    hashset_set2(&g->keywords, p, 1);
                    
                }
                struct expr_item *ei = expr_add_symbol(expr, to_symbol2(term, term_char_no), EI_IN_MATCH);
                for(int i = 0; i < term_char_no; i++){
                    symbol s = to_symbol2(&term[i], 1);
                    array_push(&ei->members, &s);
                }
                term_char_no = 0;
                p++;
                break;
            case '{':
            case '}':
                p++;
                break;
        }
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