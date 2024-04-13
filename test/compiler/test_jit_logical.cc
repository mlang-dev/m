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
#include "test_env.h"
#include "gtest/gtest.h"
#include "test_fixture.h"
#include <stdio.h>

TEST_F(TestFixture, testJITLogicalOrTrueTrue)
{
    const char test_code[] = R"(
True or True
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalOrTrueFalse)
{
    const char test_code[] = R"(
True or False
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalOrFalseFalse)
{
    const char test_code[] = R"(
False or False
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalAndTrueTrue)
{
    const char test_code[] = R"(
True or True
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalAndTrueFalse)
{
    const char test_code[] = R"(
True and False
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalAndFalseFalse)
{
    const char test_code[] = R"(
False and False
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalNotFalse)
{
    const char test_code[] = R"(
not False
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITLogicalNotTrue)
{
    const char test_code[] = R"(
not True
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(jit, block).i_value);
    node_free(block);
}
