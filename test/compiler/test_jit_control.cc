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
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITControl, testIfFunc)
{
    char test_code[] = R"(
let if_f x = 
    if x < 10 then x
    else 0
if_f 5
if_f 10
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm *)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    auto node2 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    eval_result result2 = eval_exp(jit, node2);
    ASSERT_EQ(5, result1.i_value);
    ASSERT_EQ(0, result2.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITControl, testForLoopFunc)
{
    char test_code[] = R"(
let forloop n = 
    for i in 1..n
        i
forloop 4
  )";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm *)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_back_ptr(&block->block->nodes);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    ASSERT_EQ(0, result1.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
