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

TEST(testGrammar, testOrOp)
{
    char test_grammar[] = R"(
sum         ::= sum '+' term        {}
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
    struct grammar *grammar = grammar_new(test_grammar);
    // symbol start = to_symbol("sum");
    // ASSERT_EQ(start, grammar->start_symbol);
    grammar_free(grammar);
    env_free(env);
}
