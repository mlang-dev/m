/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "clib/util.h"
#include <assert.h>

void expr_add_symbol(struct expr *expr, symbol symbol, enum atom_type type)
{
    struct atom atom;
    atom.symbol = symbol;
    atom.type = type;
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
    enum atom_type atom_type = ATOM_NONTERM;
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
                    expr_add_symbol(expr, s, is_upper(term, sizeof(term)) ? ATOM_TOKEN_MATCH : ATOM_NONTERM);
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
                term_char_no = 0;
                expr_add_symbol(expr, to_symbol2(term, term_char_no), ATOM_EXACT_MATCH);
                p++;
                break;
            case '[':
                assert(term_char_no == 0);
                while (*++p != ']') {
                    term[term_char_no++] = *p;
                }
                term_char_no = 0;
                expr_add_symbol(expr, to_symbol2(term, term_char_no), ATOM_IN_MATCH);
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
    free(grammar);
}