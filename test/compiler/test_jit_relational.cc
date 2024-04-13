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

TEST_F(TestFixture, testJITRelationalLessThan)
{
    const char test_code[] = R"(
10<11
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(true, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalLessThanWrong)
{
    char test_code[] = R"(
11<10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(false, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalGreaterThan)
{
    char test_code[] = R"(
11>10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalGreaterThanWrong)
{
    char test_code[] = R"(
10>11
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalEqual)
{
    char test_code[] = R"(
10==10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalEqualNot)
{
    char test_code[] = R"(
10==11
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalNotEqualTrue)
{
    char test_code[] = R"(
10!=11
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalNotEqualFalse)
{
    char test_code[] = R"(
10!=10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalLETrueL)
{
    char test_code[] = R"(
9<=10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalLETrueE)
{
    char test_code[] = R"(
10<=10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalLEFalse)
{
    char test_code[] = R"(
11<=10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalGETrueL)
{
    char test_code[] = R"(
10>=9
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalGETrueE)
{
    char test_code[] = R"(
10>=10
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(1, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITRelationalGEFalse)
{
    char test_code[] = R"(
10>=11
  )";
    
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(0, eval_module(jit, block).i_value);
    node_free(block);
}