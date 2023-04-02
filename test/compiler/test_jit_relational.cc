/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for logical operators JIT
 */
#include "compiler/repl.h"
#include "compiler/engine.h"
#include "codegen/llvm/cg_llvm.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "test_main.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITRelational, testLessThan)
{
    const char test_code[] = R"(
10<11
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testLessThanWrong)
{
    char test_code[] = R"(
11<10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testGreaterThan)
{
    char test_code[] = R"(
11>10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testGreaterThanWrong)
{
    char test_code[] = R"(
10>11
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testEqual)
{
    char test_code[] = R"(
10==10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testEqualNot)
{
    char test_code[] = R"(
10==11
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testNotEqualTrue)
{
    char test_code[] = R"(
10!=11
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testNotEqualFalse)
{
    char test_code[] = R"(
10!=10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testLETrueL)
{
    char test_code[] = R"(
9<=10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testLETrueE)
{
    char test_code[] = R"(
10<=10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testLEFalse)
{
    char test_code[] = R"(
11<=10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testGETrueL)
{
    char test_code[] = R"(
10>=9
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testGETrueE)
{
    char test_code[] = R"(
10>=10
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITRelational, testGEFalse)
{
    char test_code[] = R"(
10>=11
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(env->jit(), block).i_value);
    node_free(block);
}