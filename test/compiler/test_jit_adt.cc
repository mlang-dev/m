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
#include "test_env.h"
#include "test_fixture.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST_F(TestFixture, testJITAdtStructType)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:f64
let xy:Point2D = Point2D { 10.0, 20.0 }
xy.x
xy.y
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20.0, eval_module(jit, block).d_value);
    node_free(block);
}

TEST_F(TestFixture, testJITAdtProductTypeIntType)
{
    char test_code[] = R"(
struct Point2D = x:int, y:int
let xy:Point2D = Point2D { 10, 20 }
xy.x
xy.y
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITAdtProductTypeMixedType)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:int
let xy:Point2D = Point2D { 10.0, 20 }
xy.x
xy.y
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(20, eval_module(jit, block).i_value);
    node_free(block);
}

TEST_F(TestFixture, testJITAdtStructTypeMixedTypeLocalVariable)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:int
def getx():
    let xy:Point2D = Point2D { 10.0, 20 }
    xy.x
getx()
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10.0, eval_module(jit, block).d_value);
    node_free(block);
}

// TEST_F(TestFixture, testJITAdtStructEmbedStruct)
// {
//     char test_code[] = R"(
// struct xy = x:f64, y:f64
// struct wz = w:f64, z:mut xy
// let ab = wz{10.0, xy{20.0, 30.0}}
// ab.z = xy{200.0, 300.0}
// ab.z.y
// )";
//     TestEnvironment *env = get_env();
//     engine_reset(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     ASSERT_EQ(300.0, eval_module(jit, block).d_value);
//     node_free(block);
// }

TEST_F(TestFixture, testJITAdtTuple_type)
{
    char test_code[] = R"(
let x = (10, 20)
x[0]
)";
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    ASSERT_EQ(10, eval_module(jit, block).i_value);
    node_free(block);
}
