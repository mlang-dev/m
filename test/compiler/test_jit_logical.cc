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

TEST(testJITLogical, testOrTrueTrue)
{
    const char test_code[] = R"(
true || true
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testOrTrueFalse)
{
    const char test_code[] = R"(
true || false
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testOrFalseFalse)
{
    const char test_code[] = R"(
false || false
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testAndTrueTrue)
{
    const char test_code[] = R"(
true && true
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testAndTrueFalse)
{
    const char test_code[] = R"(
true && false
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testAndFalseFalse)
{
    const char test_code[] = R"(
false && false
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testNotFalse)
{
    const char test_code[] = R"(
! false
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(true, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testJITLogical, testNotTrue)
{
    const char test_code[] = R"(
! true
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node);
    ASSERT_EQ(false, result.i_value);
    ast_node_free(block);
    engine_free(engine);
}
