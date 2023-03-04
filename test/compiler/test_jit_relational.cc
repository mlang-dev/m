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
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITRelational, testLessThan)
{
    const char test_code[] = R"(
10<11
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    node_free(block);
    engine_free(engine);
}

TEST(testJITRelational, testLessThanWrong)
{
    char test_code[] = R"(
11<10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    engine_free(engine);
}

TEST(testJITRelational, testGreaterThan)
{
    char test_code[] = R"(
11>10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    engine_free(engine);
}

TEST(testJITRelational, testGreaterThanWrong)
{
    char test_code[] = R"(
10>11
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testEqual)
{
    char test_code[] = R"(
10==10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testEqualNot)
{
    char test_code[] = R"(
10==11
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testNotEqualTrue)
{
    char test_code[] = R"(
10!=11
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testNotEqualFalse)
{
    char test_code[] = R"(
10!=10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testLETrueL)
{
    char test_code[] = R"(
9<=10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testLETrueE)
{
    char test_code[] = R"(
10<=10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testLEFalse)
{
    char test_code[] = R"(
11<=10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testGETrueL)
{
    char test_code[] = R"(
10>=9
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testGETrueE)
{
    char test_code[] = R"(
10>=10
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITRelational, testGEFalse)
{
    char test_code[] = R"(
10>=11
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(0, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
