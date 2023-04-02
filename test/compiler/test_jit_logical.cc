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

TEST(testJITLogical, testOrTrueTrue)
{
    const char test_code[] = R"(
true || true
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testOrTrueFalse)
{
    const char test_code[] = R"(
true || false
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testOrFalseFalse)
{
    const char test_code[] = R"(
false || false
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testAndTrueTrue)
{
    const char test_code[] = R"(
true && true
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testAndTrueFalse)
{
    const char test_code[] = R"(
true && false
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testAndFalseFalse)
{
    const char test_code[] = R"(
false && false
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testNotFalse)
{
    const char test_code[] = R"(
! false
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITLogical, testNotTrue)
{
    const char test_code[] = R"(
! true
  )";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(env->jit(), block).i_value);
    node_free(block);
}
