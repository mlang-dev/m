/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT for control statement and expressions
 */
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "compiler/repl.h"
#include "sema/analyzer.h"
#include "parser/ast.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include "test_env.h"
#include <stdio.h>

#include "llvm-c/Core.h"
#include "llvm-c/Error.h"
#include "llvm-c/LLJIT.h"
#include "llvm-c/Support.h"
#include "llvm-c/Target.h"
#include "test_fixture.h"

TEST_F(TestFixture, testJITControlIfFunc)
{
    char test_code[] = R"(
def if_f(x):
    if x < 10: x
    else: 0
if_f(5)
if_f(10)
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlForLoopFunc)
{
    char test_code[] = R"(
def forloop(n):
    let mut j = 0
    for i in 1..n:
        j += i
    j
forloop(5)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(10, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlBreakForLoop)
{
    char test_code[] = R"(
def forloop(n, m):
    let mut j = 0
    for i in 1..n:
        j += i
        if i == m:
            break
    j
forloop(5, 3)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(6, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlContinueForLoop)
{
    char test_code[] = R"(
def forloop(n, m): 
    let mut j = 0
    for i in 0..n:
        if i%2 == 0:
            continue
        j += i
        if i == m:
            break
    j
forloop(5, 3)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(4, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlReturnForLoop)
{
    char test_code[] = R"(
def forloop(n, m): 
    let mut j = 0
    for i in 0..n:
        j += i
        if i == m:
            return 100
    j
forloop(5, 3)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(100, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlWhileLoop)
{
    char test_code[] = R"(
def loopfunc(m): 
    let mut i = 0
    let mut j = 0
    while i < m:
        if i%2 == 0:
            i++
            continue
        j += i
        i++
    j
loopfunc(5)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(4, result.i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITControlWhile_break_Loop)
{
    char test_code[] = R"(
def loopfunc(m): 
    let mut i = 0
    let mut j = 0
    while True:
        if i%2 == 0:
            i++
            continue
        if i >= m:
            break
        j += i
        i++
    j
loopfunc(5)
  )";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(4, result.i_value);
    node_free(block);
}
