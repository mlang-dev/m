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
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    auto node4 = (struct ast_node *)array_get_ptr(&block->block->nodes, 3);
    ASSERT_EQ(20.0, eval_exp(jit, node4).d_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITAdt, testProductTypeIntType)
{
    char test_code[] = R"(
struct Point2D = x:int, y:int
let xy:Point2D = Point2D { 10, 20 }
xy.x
xy.y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(10, eval_exp(jit, node3).i_value);
    auto node4 = (struct ast_node *)array_get_ptr(&block->block->nodes, 3);
    ASSERT_EQ(20, eval_exp(jit, node4).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITAdt, testProductTypeMixedType)
{
    char test_code[] = R"(
struct Point2D = x:f64, y:int
let xy:Point2D = Point2D { 10.0, 20 }
xy.x
xy.y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    auto node4 = (struct ast_node *)array_get_ptr(&block->block->nodes, 3);
    ASSERT_EQ(20, eval_exp(jit, node4).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
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
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITAdt, tuple_type)
{
    char test_code[] = R"(
let x = (10, 20)
x[0]
)";
    struct engine *engine = engine_llvm_new(true);
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block, 0);
    ASSERT_EQ(10, eval_module(jit, block).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
