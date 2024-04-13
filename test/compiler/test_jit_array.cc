/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT for control statement and expressions
 */
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "compiler/repl.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "test_env.h"
#include "test_fixture.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST_F(TestFixture, testJITArrayArray_global_array_access)
{
    char test_code[] = R"(
let a = [10]
a[0]
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITArrayArray_local_array_access)
{
    char test_code[] = R"(
def f():
    let a = [10]
    a[0]
f()
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITArrayArray_local_array_write_read)
{
    char test_code[] = R"(
def f():
    let mut a = [10]
    a[0] = 20
    a[0]
f()
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(jit, block).i_value);
    node_free(block);
}
