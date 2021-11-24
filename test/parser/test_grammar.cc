/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/m_grammar.h"
#include "parser/parser.h"
#include "gtest/gtest.h"
#include <stdio.h>
#include "clib/hashtable.h"

TEST(testGrammar, testNumToken)
{
    char test_grammar[] = R"(
power       = NUM 
    )";
    symbols_init();
    struct grammar *grammar = grammar_parse(test_grammar);
    symbol start = to_symbol2("power", 5);
    ASSERT_EQ(start, grammar->start_symbol);
    ASSERT_EQ(1, array_size(&grammar->rules));
    int expected_exps[] = { 1 };
    int expected_items[1][1] = {
        { 1 },
    };
    for (size_t i = 0; i < array_size(&grammar->rules); i++) {
        struct rule *rule = *(struct rule **)array_get(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++) {
            struct expr *expr = (struct expr *)array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
        }
    }
    struct rule *rule = (struct rule *)hashtable_get_p(&grammar->rule_map, start);
    struct expr *expr = (struct expr*)array_front(&rule->exprs);
    struct expr_item *ei = (struct expr_item*)array_front(&expr->items);
    ASSERT_EQ(ei->ei_type, EI_TOKEN_MATCH);
    ASSERT_EQ(ei->sym, to_symbol2("NUM", 3));
    ASSERT_EQ(0, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    symbols_deinit();
}

TEST(testGrammar, testArithmeticExp)
{
    char test_grammar[] = R"(
sum         = sum '+' term      {}
            | sum '-' term      {}
            | term              {}
term        = term '*' factor   {}
            | term '/' factor   {}
            | term '%' factor   {}
            | factor            {}
factor      = '+' factor        {}
            | '-' factor        {}
            | power             {}
power       = NUM '^' factor    {}
            | NUM               {}

    )";
    symbols_init();
    struct grammar *grammar = grammar_parse(test_grammar);
    symbol start = to_symbol2("sum", 3);
    ASSERT_EQ(start, grammar->start_symbol);
    ASSERT_EQ(4, array_size(&grammar->rules));
    int expected_exps[] = {3, 4, 3, 2};
    int expected_items[4][4] = {
        {3, 3, 1, 0}, 
        {3, 3, 3, 1},
        {2, 2, 1, 0},
        {3, 1, 0, 0}
    };
    for(size_t i = 0; i < array_size(&grammar->rules); i++){
        struct rule *rule = *(struct rule**)array_get(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++){
            struct expr *expr = (struct expr *)array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
        }
    }
    ASSERT_EQ(6, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    symbols_deinit();
}

TEST(testGrammar, testArithmeticExpUsingCharSetOr)
{
    char test_grammar[] = R"(
sum         = sum [+-] term     { binop 0 1 2 }
            | term              { 0 }
term        = term [*/%] factor { binop 0 1 2 }
            | factor            { 0 }
factor      = '(' sum ')'       { 1 }
            | [+-] factor       { unop 0 1 }
            | power             { 0 }
power       = NUM '^' factor    { binop 0 1 2 }
            | NUM               { 0 }

    )";
    symbols_init();
    struct grammar *grammar = grammar_parse(test_grammar);
    symbol start = to_symbol2("sum", 3);
    ASSERT_EQ(start, grammar->start_symbol);
    ASSERT_EQ(4, array_size(&grammar->rules));
    int expected_exps[] = { 2, 2, 3, 2 };
    int expected_items[4][4] = {
        { 3, 1, 0, 0 },
        { 3, 1, 0, 0 },
        { 3, 2, 1, 0 },
        { 3, 1, 0, 0 }
    };
    int expected_actions[4][4] = {
        { 3, 1, 0, 0 },
        { 3, 1, 0, 0 },
        { 1, 2, 1, 0 },
        { 3, 1, 0, 0 }
    };
    for (size_t i = 0; i < array_size(&grammar->rules); i++) {
        struct rule *rule = *(struct rule **)array_get(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++) {
            struct expr *expr = (struct expr *)array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
            ASSERT_EQ(expected_actions[i][j], expr->action.exp_item_index_count);
        }
    }
    ASSERT_EQ(8, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    symbols_deinit();
}
