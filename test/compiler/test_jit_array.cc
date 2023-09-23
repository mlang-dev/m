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
#include "test_main.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITArray, array_global_array_access)
{
    char test_code[] = R"(
let a = [10]
a[0]
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITArray, array_local_array_access)
{
    char test_code[] = R"(
def f() =
    let a = [10]
    a[0]
f()
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITArray, array_local_array_write_read)
{
    char test_code[] = R"(
def f() =
    let mut a = [10]
    a[0] = 20
    a[0]
f()
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(env->jit(), block).i_value);
    node_free(block);
}
