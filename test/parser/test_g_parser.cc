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

char test_grammar[] = R"(
sum         = sum [+-] term     { expr 0 1 2 }
            | term              { 0 }
term        = term [*/%] factor { expr 0 1 2 }
            | factor            { 0 }
factor      = '(' sum ')'       { 1 }
            | [+-] factor       { unary 0 1 }
            | power             { 0 }
power       = NUM '^' factor    { expr 0 1 2 }
            | NUM               { 0 }
    )";

TEST(testGParser, testArithmeticExp)
{
    const char test_code[] = "1+2";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = print(ast, test_code);
    ASSERT_STREQ("1 + 2", to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp1)
{
    const char test_code[] = "(1+2)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = print(ast, test_code);
    ASSERT_STREQ("( 1 + 2 )", to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}
