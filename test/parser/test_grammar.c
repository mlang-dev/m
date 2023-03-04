/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "test.h"
#include <stdio.h>
#include "clib/hashtable.h"
#include "sema/frontend.h"

TEST(test_grammar, num_token)
{
    char test_grammar[] = "power       = INT";
    struct frontend *fe = frontend_init();
    struct grammar *grammar = grammar_parse(test_grammar);
    symbol start = to_symbol2("power", 5);
    ASSERT_EQ(start, grammar->start_symbol);
    ASSERT_EQ(1, array_size(&grammar->rules));
    int expected_exps[] = { 1 };
    int expected_items[1][1] = {
        { 1 },
    };
    for (size_t i = 0; i < array_size(&grammar->rules); i++) {
        struct rule *rule = array_get_ptr(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++) {
            struct expr *expr = array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
        }
    }
    struct rule *rule = (struct rule *)hashtable_get_p(&grammar->rule_map, start);
    struct expr *expr = array_front(&rule->exprs);
    struct expr_item *ei = array_front(&expr->items);
    ASSERT_EQ(ei->ei_type, EI_TOKEN_MATCH);
    ASSERT_EQ(ei->sym, to_symbol2("INT", 3));
    ASSERT_EQ(0, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    frontend_deinit(fe);
}

TEST(test_grammar, arithmetic_exp)
{
    char test_grammar[] = 
"sum         = sum '+' term      {}\n"
"            | sum '-' term      {}\n"
"            | term              {}\n"
"term        = term '*' factor   {}\n"
"            | term '/' factor   {}\n"
"            | term '%' factor   {}\n"
"            | factor            {}\n"
"factor      = '+' factor        {}\n"
"            | '-' factor        {}\n"
"            | power             {}\n"
"power       = INT '^' factor    {}\n"
"            | INT               {}\n";

    struct frontend *fe = frontend_init();
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
        struct rule *rule = array_get_ptr(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++){
            struct expr *expr = array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
        }
    }
    ASSERT_EQ(6, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    frontend_deinit(fe);
}

TEST(test_grammar, arithmetic_exp_using_charset)
{
    char test_grammar[] = 
"\n"
"sum         = sum [+-] term     { binop 0 1 2 }\n"
"            | term              { 0 }\n"
"term        = term [*/%] factor { binop 0 1 2 }\n"
"            | factor            { 0 }\n"
"factor      = '(' sum ')'       { 1 }\n"
"            | [+-] factor       { unop 0 1 }\n"
"            | power             { 0 }\n"
"power       = INT '^' factor    { binop 0 1 2 }\n"
"            | INT               { 0 }\n";
    struct frontend *fe = frontend_init();
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
        struct rule *rule = array_get_ptr(&grammar->rules, i);
        ASSERT_EQ(expected_exps[i], array_size(&rule->exprs));
        for (size_t j = 0; j < array_size(&rule->exprs); j++) {
            struct expr *expr = array_get(&rule->exprs, j);
            ASSERT_EQ(expected_items[i][j], array_size(&expr->items));
            ASSERT_EQ(expected_actions[i][j], expr->action.exp_item_index_count);
        }
    }
    ASSERT_EQ(8, hashset_size(&grammar->keywords));
    grammar_free(grammar);
    frontend_deinit(fe);
}

int test_grammar()
{
    UNITY_BEGIN();
    RUN_TEST(test_grammar_num_token);
    RUN_TEST(test_grammar_arithmetic_exp);
    RUN_TEST(test_grammar_arithmetic_exp_using_charset);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
