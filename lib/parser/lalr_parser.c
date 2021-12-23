/*
 * lalr_parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a LALR parser, taking a EBNF grammar text and parse text 
 * into ast for the grammar
 */
#include "parser/lalr_parser.h"
#include "clib/stack.h"
#include "clib/util.h"
#include "parser/grammar.h"
#include <assert.h>

/*expand bracket in grammar*/
void _expr_2_gr(struct expr *expr, struct grule *gr)
{
    struct expr_item *ei;
    gr->symbol_count = 0;
    for(size_t i=0; i < array_size(&expr->items); i++){
        assert(i < MAX_SYMBOLS_RULE);
        ei = (struct expr_item *)array_get(&expr->items, i);
        gr->rhs[gr->symbol_count++] = get_symbol_index(ei->sym);
    }
}

void _expand_expr(struct expr *rule_expr, struct array *a)
{
    int which_in_items = -1;
    size_t i;
    struct expr_item *ei;
    struct array symbols;
    symbol s;
    array_init(&symbols, sizeof(symbol));
    for(i = 0; i < array_size(&rule_expr->items); i++){
        ei = array_get(&rule_expr->items, i);
        if(ei->ei_type == EI_IN_MATCH){
            which_in_items = i;
            const char *sym = string_get(ei->sym);
            while(*sym){
                s = to_symbol2(sym, 1);
                array_push(&symbols, &s);
                sym++;
            }
            break;
        }
    }
    struct expr *expr;
    if(which_in_items < 0){
        array_push(a, rule_expr);
        expr = array_back(a);
        array_copy(&expr->items, &rule_expr->items);
    }else{
        for(i=0; i < array_size(&symbols); i++){
            array_push(a, rule_expr);
            expr = array_back(a);
            array_copy(&expr->items, &rule_expr->items);
            ei = array_get(&expr->items, which_in_items);
            ei->ei_type = EI_TOKEN_MATCH;
            ei->sym = *(symbol*)array_get(&symbols, i);
        }
    }
    array_deinit(&symbols);
}

struct lalr_parser *lalr_parser_new(const char *grammar_text)
{
    size_t i,j,k;
    struct lalr_parser *parser;
    MALLOC(parser, sizeof(*parser));
    parser->stack_top = 0;
    //1. initialize parsing table
    for(i=0; i < MAX_STATES; i++){
        for(j=0; j < MAX_GRAMMAR_SYMBOLS; j++){
            parser->parsing_table[i][j].code = ACTION_ERROR;
            parser->parsing_table[i][j].state_index = 0;
        }
    }
    //2. add rules
    struct grammar *g = grammar_parse(grammar_text);
    
    struct rule *rule;
    u8 nonterm;
    for(i = 0; i < array_size(&g->rules); i++){
        rule = *(struct rule **)array_get(&g->rules, i);
        nonterm = (u8)i+PATTERN_COUNT;
        u8 index = add_grammar_nonterm(rule->nonterm);
        assert(nonterm == index);
    }

    //3. convert grammar symbol to use index
    struct grule *gr;
    struct expr *rule_expr, *expr;
    parser->rule_count = 0;
    for(i = 0; i < (u16)array_size(&g->rules); i++){
        rule = *(struct rule **)array_get(&g->rules, i);
        nonterm = get_symbol_index(rule->nonterm);
        for(j = 0; j < array_size(&rule->exprs); j++){
            rule_expr = (struct expr *)array_get(&rule->exprs, j);
            struct array exprs;
            array_init_free(&exprs, sizeof(struct expr), (free_fun)expr_deinit);
            _expand_expr(rule_expr, &exprs);
            for(k = 0; k < array_size(&exprs); k++){
                expr = array_get(&exprs, k);
                gr = &parser->rules[parser->rule_count++];
                gr->lhs = nonterm;
                _expr_2_gr(expr, gr);
            }
            array_deinit(&exprs);
        }
    }

    //4. build states

    //5. construct parsing table

    parser->g = g;
    return parser;
}

void lalr_parser_free(struct lalr_parser *parser)
{
    grammar_free(parser->g);
    FREE(parser);
}

struct ast_node *parse_text(struct lalr_parser *parser, const char *text)
{
    //driver 
    return 0;
}