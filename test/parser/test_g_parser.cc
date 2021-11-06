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


TEST(testGParser, testArithmeticExp)
{
    char test_grammar[] = R"(
sum         = sum [+-] term     {}
            | term              {}
term        = term [*/%] factor {}
            | factor            {}
factor      = '(' sum ')'       {}
            | [+-] factor       {}
            | power             {}
power       = NUM '^' factor    {}
            | NUM               {}

    )";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    ASSERT_EQ(true, parser_parse(parser, ""));
    parser_free(parser);
    env_free(env);
}
