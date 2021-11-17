/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "codegen/env.h"
#include "parser/grammar.h"
#include "parser/parser.h"
#include "codegen/wasm_codegen.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

char test_grammar[] = R"(
sum         = sum [+-] term     { binop 0 1 2 }
            | term              { 0 }
term        = term [*/%] factor { binop 0 1 2 }
            | factor            { 0 }
factor      = '(' sum ')'       { 1 }
            | NUM               { 0 }
    )";

TEST(testGParser, testArithmeticExp)
{
    const char test_code[] = "1+2";
    const char expected[] = R"(
i32.const 1
i32.const 2
i32.add
)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp1)
{
    const char test_code[] = "(1+2)";
    const char expected[] = R"(
i32.const 1
i32.const 2
i32.add
)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp2)
{
    const char test_code[] = "0 + 2 * 4";
    const char expected[] = R"(
i32.const 0
i32.const 2
i32.const 4
i32.mul
i32.add
)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp3)
{
    const char test_code[] = "1 * 2 + 3";
    const char expected[] = R"(
i32.const 1
i32.const 2
i32.mul
i32.const 3
i32.add
)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp4)
{
    const char test_code[] = "(1 + 2) * 3";
    const char expected[] = R"(
i32.const 1
i32.const 2
i32.add
i32.const 3
i32.mul
)";
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    env_free(env);
}
