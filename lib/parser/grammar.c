/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "clib/util.h"
#include <assert.h>

void expr_item_init(struct expr_item *ei, symbol sym, enum expr_item_type type)
{
    ei->sym = sym;
    ei->ei_type = type;
}

struct expr_item* expr_add_symbol(struct expr *expr, symbol sym, enum expr_item_type type)
{
    struct expr_item ei;
    expr_item_init(&ei, sym, type);
    array_push(&expr->items, &ei);
    return (struct expr_item*)array_back(&expr->items);
}

bool expr_item_exists_symbol(struct expr_item *ei, char sym)
{
    const char *p = string_get(ei->sym);
    for(size_t i = 0; i < string_size(ei->sym); i++){
        if(p[i] == sym) return true;
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
    const char meta_chars[] = "=|[]{}";
    symbol DEFINITION = to_symbol2(&meta_chars[0], 1);
    symbol ALTERNATION = to_symbol2(&meta_chars[1], 1);
    symbol OPTION_S = to_symbol2(&meta_chars[2], 1);
    symbol OPTION_E = to_symbol2(&meta_chars[3], 1);
    symbol ACTION_S = to_symbol2(&meta_chars[4], 1);
    symbol ACTION_E = to_symbol2(&meta_chars[5], 1);
    while (tok.tok_type) {
        get_tok(&lexer, &next_tok);
        if(tok.tok_type_name == IDENT_TOKEN){
            s = to_symbol2(&grammar_text[tok.loc.start], tok.loc.end - tok.loc.start);
            if(next_tok.tok_type_name == DEFINITION){
                //nonterm
                rule = grammar_add_rule(g, s, rule_no++);
                expr = rule_add_expr(rule);
                get_tok(&lexer, &next_tok); //skip the '='
            }else if(expr){
                expr_add_symbol(expr, s, is_upper((string*)s) ? EI_TOKEN_MATCH : EI_NONTERM);
            }
        } else if (tok.tok_type_name == STRING_TOKEN || tok.tok_type_name == CHAR_TOKEN) {
            s = to_symbol2(&grammar_text[tok.loc.start+1], tok.loc.end - tok.loc.start - 2); //stripping off two single quote
            expr_add_symbol(expr, s, EI_EXACT_MATCH);
            hashset_set2(&g->keywords, string_get(s), string_size(s));
        }
        if(tok.tok_type_name == ALTERNATION){
            expr = rule_add_expr(rule);
        } else if (tok.tok_type_name == OPTION_S){ //regex
            string_init_chars2(&group, "", 0);
            while(next_tok.tok_type_name != OPTION_E){
                const char *p = &grammar_text[next_tok.loc.start];
                for(int i = 0; i < next_tok.loc.end - next_tok.loc.start; i ++){
                    hashset_set2(&g->keywords, p + i, 1);
                }
                string_add_chars2(&group, p, next_tok.loc.end - next_tok.loc.start);
                get_tok(&lexer, &next_tok);
            }
            expr_add_symbol(expr, string_2_symbol2(&group), EI_IN_MATCH);
            get_tok(&lexer, &next_tok); // skip ']'
        }else if (tok.tok_type_name == ACTION_S){
            while(next_tok.tok_type_name != ACTION_E){
                //next tok is action
                if(next_tok.tok_type_name == IDENT_TOKEN){
                    assert(expr->action.action == 0);
                    expr->action.action  = to_symbol2(&grammar_text[next_tok.loc.start], next_tok.loc.end - next_tok.loc.start);
                }
                else if(next_tok.tok_type_name == NUM_TOKEN){
                    expr->action.exp_item_index[expr->action.exp_item_index_count++] = grammar_text[next_tok.loc.start] - '0'; 
                }
                else
                    assert(false);
                get_tok(&lexer, &next_tok);//'}'
            }
            get_tok(&lexer, &next_tok); //skip '}
        }
        tok = next_tok;
        next_tok.tok_type_name = 0;
    }
    if (array_size(&g->rules)) {
        g->start_symbol = (*(struct rule **)array_front(&g->rules))->nonterm;
    }
    return g;
}

int expr_item_2_ast_node_index(struct expr *expr, int expr_item_index)
{
    for(int i = 0; i < expr->action.exp_item_index_count; i++){
        if (expr->action.exp_item_index[i] == expr_item_index)
            return i;
    }
    return -1;
}

string print_rule_expr(symbol nonterm, struct expr *expr)
{
    string s;
    string_init_chars2(&s, string_get(nonterm), string_size(nonterm));
    string_add_chars2(&s, " =", 2);
    for(size_t i = 0; i < array_size(&expr->items); i++){
        struct expr_item *ei = (struct expr_item *)array_get(&expr->items, i);
        string_add_chars2(&s, " ", 1);
        string_add_chars2(&s, string_get(ei->sym), string_size(ei->sym));
    }
    return s;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rule_map);
    array_deinit(&grammar->rules);
    hashset_deinit(&grammar->keywords);
    FREE(grammar);
}
