/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "pgen/grammar.h"
#include "lexer/lexer.h"
#include "pgen/lang_token.h"
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
    return array_back(&rule->exprs);
}

void rule_deinit(struct rule *rule)
{
    for (size_t i = 0; i < array_size(&rule->exprs); i++) {
        struct expr *expr = array_get(&rule->exprs, i);
        expr_deinit(expr);
    }
    array_deinit(&rule->exprs);
}

struct grammar *_grammar_new(void)
{
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashset_init(&grammar->keywords);
    hashtable_init_with_value_size(&grammar->rule_map, sizeof(struct rule), (free_fun)rule_deinit);
    array_init(&grammar->rules, sizeof(struct rule *));
    grammar->start_symbol = 0;
    grammar->token_count = 0;
    grammar->op_count = 0;
    grammar->terminal_count = 0;
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

u32 _parse_token(const char *token_text)
{
    if (!token_text) return 0;
    struct lexer *lexer= lexer_new_for_string(token_text);
    struct token tok;
    u32 token_count = 0;
    tok = *get_tok(lexer);
    bool token_open = false;
    while (tok.token_type!=TOKEN_EOF) {
        tok = *get_tok(lexer);
        if(tok.token_type == TOKEN_IDENT && !token_open){
            token_open = true;
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_LPAREN);
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_IDENT);//op name
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_COMMA);
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_LITERAL_STRING);
            const char *name = tok.str_val;
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_COMMA);
            tok = *get_tok(lexer);
            const char *pattern = 0;
            if(tok.token_type == TOKEN_LITERAL_STRING){
                pattern = tok.str_val;
            }
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_COMMA);
            tok = *get_tok(lexer);
            const char *class_name = 0;
            if(tok.token_type == TOKEN_LITERAL_STRING){
                class_name = tok.str_val;
            }
            create_lang_token_pattern(token_count, name, pattern, class_name);
        }
        if(tok.token_type == TOKEN_RPAREN && token_open){
            token_open = false;
            token_count ++;
        }
    }
    lexer_free(lexer);
    return token_count;
}

u32 _parse_op(const char *op_text, u16 token_op)
{
    if (!op_text) return 0;
    struct lexer *lexer= lexer_new_for_string(op_text);
    struct token tok;
    u32 op_count = 1; //OP_NULL is default 0
    tok = *get_tok(lexer);
    bool token_open = false;
    while (tok.token_type!=TOKEN_EOF) {
        tok = *get_tok(lexer);
        if(tok.token_type == TOKEN_IDENT && !token_open){
            token_open = true;
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_LPAREN);
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_IDENT);//op name
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_COMMA);
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_LITERAL_STRING);
            const char *name = tok.str_val;
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_COMMA);
            tok = *get_tok(lexer);
            assert(tok.token_type == TOKEN_LITERAL_STRING);
            create_lang_op_pattern(token_op, op_count, name, tok.str_val);
        }
        if(tok.token_type == TOKEN_RPAREN && token_open){
            token_open = false;
            op_count ++;
        }
    }
    lexer_free(lexer);
    return op_count;
}

struct grammar *grammar_parse(const char *grammar_text, const char *token_text, const char *op_text)
{
    struct grammar *g = _grammar_new();
    g->token_count = _parse_token(token_text);
    g->op_count = _parse_op(op_text, g->token_count - 1); //the last is TOKEN_OP
    g->terminal_count = g->token_count - 1 + g->op_count;
    struct token tok, next_tok;
    struct lexer *lexer= lexer_new_for_string(grammar_text);
    tok = *get_tok(lexer);
    int rule_no = 0;
    struct rule *rule = 0;
    struct expr *expr = 0;
    symbol s = 0;
    string group;
    while (tok.token_type!=TOKEN_EOF) {
        next_tok = *get_tok(lexer);
        if(tok.token_type == TOKEN_IDENT){
            s = tok.symbol_val;
            if(next_tok.token_type == TOKEN_OP && next_tok.opcode == OP_ASSIGN){
                //nonterm
                rule = grammar_add_rule(g, s, rule_no++);
                expr = rule_add_expr(rule);
                next_tok = *get_tok(lexer); //skip the '='
            }else if(expr){
                expr_add_symbol(expr, s, is_upper((string*)s) ? EI_TOKEN_MATCH : EI_NONTERM);
            }
        } else if (tok.token_type == TOKEN_LITERAL_STRING || tok.token_type == TOKEN_LITERAL_CHAR) {
            if(tok.token_type == TOKEN_LITERAL_CHAR)
                s = to_symbol2((char*)&tok.int_val, 1);
            else
                s = to_symbol(tok.str_val);
            expr_add_symbol(expr, s, EI_EXACT_MATCH);
            hashset_set2(&g->keywords, string_get(s), string_size(s));
        }
        if(tok.opcode == OP_BITOR){
            expr = rule_add_expr(rule);
        } else if (tok.token_type == TOKEN_LBRACKET){ //regex
            string_init_chars2(&group, "", 0);
            while(next_tok.token_type != TOKEN_RBRACKET){
                const char *p = &grammar_text[next_tok.loc.start];
                for(int i = 0; i < next_tok.loc.end - next_tok.loc.start; i ++){
                    hashset_set2(&g->keywords, p + i, 1);
                }
                string_add_chars2(&group, p, next_tok.loc.end - next_tok.loc.start);
                next_tok = *get_tok(lexer);
            }
            expr_add_symbol(expr, string_2_symbol(&group), EI_IN_MATCH);
            next_tok = *get_tok(lexer); // skip ']'
        }else if (tok.token_type == TOKEN_LCBRACKET){
            while(next_tok.token_type != TOKEN_RCBRACKET){
                //next tok is action
                if(next_tok.token_type == TOKEN_IDENT){
                    assert(expr->action.action == 0);
                    expr->action.action = next_tok.symbol_val;
                }
                else if(next_tok.token_type == TOKEN_LITERAL_INT){
                    expr->action.exp_item_index[expr->action.exp_item_index_count++] = next_tok.int_val;
                }
                else{    
                    printf("unaccepted token type: %d\n", next_tok.token_type);
                    assert(false);
                }
                next_tok = *get_tok(lexer); // skip ']'
            }
            next_tok = *get_tok(lexer); // skip ']'
        }
        tok = next_tok;
        next_tok.token_type = TOKEN_EOF;
    }
    if (array_size(&g->rules)) {
        g->start_symbol = ((struct rule *)array_front_ptr(&g->rules))->nonterm;
    }
    lexer_free(lexer);
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
        struct expr_item *ei = array_get(&expr->items, i);
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
