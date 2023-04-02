/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT using algebraic data type
 */
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "compiler/repl.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "test_main.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITAdt, testStructType)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:f64
let xy:Point2D = Point2D { 10.0, 20.0 }
xy.x
xy.y
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20.0, eval_module(env->jit(), block).d_value);
    node_free(block);
}

TEST(testJITAdt, testProductTypeIntType)
{
    char test_code[] = R"(
struct Point2D = x:int, y:int
let xy:Point2D = Point2D { 10, 20 }
xy.x
xy.y
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITAdt, testProductTypeMixedType)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:int
let xy:Point2D = Point2D { 10.0, 20 }
xy.x
xy.y
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(env->jit(), block).i_value);
    node_free(block);
}

TEST(testJITAdt, testStructTypeMixedTypeLocalVariable)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:int
let getx()=
    let xy:Point2D = Point2D { 10.0, 20 }
    xy.x
getx()
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10.0, eval_module(env->jit(), block).d_value);
    node_free(block);
}

TEST(testJITAdt, tuple_type)
{
    char test_code[] = R"(
let x = (10, 20)
x[0]
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(env->jit(), block).i_value);
    node_free(block);
}
