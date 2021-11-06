/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "codegen/env.h"
#include "parser/grammar.h"
#include "parser/parser.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGrammar, testArithmeticExp)
{
    char test_grammar[] = R"(
sum         ::= sum [ab] term        {}
                | sum '-' term      {}
                | term              {}
term        ::= term '*' factor     {}
                | term '/' factor   {}
                | term '%' factor   {}
                | factor            {}
factor      ::= '+' factor          {}
                | '-' factor        {}
                | power             {}
power       ::= NUM '^' factor      {}
                | NUM               {}

    )";
    struct env *env = env_new(false);
    struct grammar *grammar = grammar_parse(test_grammar);
    symbol start = to_symbol("sum");
    ASSERT_EQ(start, grammar->start_symbol);
    ASSERT_EQ(4, array_size(&grammar->rules));
    int expected_exps[] = {3, 4, 3, 2};
    int expected_atoms[4][4] = {
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
            ASSERT_EQ(expected_atoms[i][j], array_size(&expr->atoms));
        }
    }
    grammar_free(grammar);
    env_free(env);
}
