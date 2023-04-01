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
#include "test_main.h"
#include <stdio.h>

// TEST(testJITControl, testIfFunc)
// {
//     char test_code[] = R"(
// let if_f x = 
//     if x < 10 then x
//     else 0
// if_f 5
// if_f 10
// )";
//     struct engine *engine = engine_llvm_new(false);
//     struct cg_llvm *cg = (struct cg_llvm *)engine->be->cg;
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     analyze(cg->base.sema_context, block);
//     auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
//     auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
//     auto node2 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
//     eval_statement(jit, node);
//     eval_result result1 = eval_exp(jit, node1);
//     eval_result result2 = eval_exp(jit, node2);
//     ASSERT_EQ(5, result1.i_value);
//     ASSERT_EQ(0, result2.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

// TEST(testJITControl, testForLoopFunc)
// {
//     char test_code[] = R"(
// let forloop n = 
//     var j = 0
//     for i in 1..n
//         j += i
//     j
// forloop 5
//   )";
//     struct engine *engine = engine_llvm_new(false);
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     eval_result result = eval_module(jit, block);
//     ASSERT_EQ(10, result.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

// TEST(testJITControl, breakForLoop)
// {
//     char test_code[] = R"(
// let forloop n m = 
//     var j = 0
//     for i in 1..n
//         j += i
//         if i == m then
//             break
//     j
// forloop 5 3
//   )";
//     struct engine *engine = engine_llvm_new(false);
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     eval_result result = eval_module(jit, block);
//     ASSERT_EQ(6, result.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

// TEST(testJITControl, continueForLoop)
// {
//     char test_code[] = R"(
// let forloop n m = 
//     var j = 0
//     for i in 0..n
//         if i%2 == 0 then
//             continue
//         j += i
//         if i == m then
//             break
//     j
// forloop 5 3
//   )";
//     struct engine *engine = engine_llvm_new(false);
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     eval_result result = eval_module(jit, block);
//     ASSERT_EQ(4, result.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

// TEST(testJITControl, returnForLoop)
// {
//     char test_code[] = R"(
// let forloop n m = 
//     var j = 0
//     for i in 0..n
//         j += i
//         if i == m then
//             return 100
//     j
// forloop 5 3
//   )";
//     struct engine *engine = engine_llvm_new(false);
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     eval_result result = eval_module(jit, block);
//     ASSERT_EQ(100, result.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

// TEST(testJITControl, whileLoop)
// {
//     char test_code[] = R"(
// let loopfunc n m = 
//     var j = 0
//     var i = 0
//     while i < m
//         i++
//         if i%2 == 0 then
//             continue
//         j = j + i
//     j
// loopfunc 5 3
//   )";
//     struct engine *engine = engine_llvm_new(false);
//     JIT *jit = build_jit(engine);
//     struct ast_node *block = parse_code(engine->fe->parser, test_code);
//     block = split_ast_nodes_with_start_func(0, block);
//     // eval_result result = eval_module(jit, block);
//     // ASSERT_EQ(4, result.i_value);
//     node_free(block);
//     jit_free(jit);
//     engine_free(engine);
// }

TEST(testJITControl, whilebreakLoop)
{
    char test_code[] = R"(
4
)";
    Environment *env = get_env();
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(env->jit(), block);
    ASSERT_EQ(4, result.i_value);
    node_free(block);
}
