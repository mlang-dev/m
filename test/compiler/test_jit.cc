/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT
 */
#include <stdio.h>
#include "compiler/repl.h"
#include "compiler/engine.h"
#include "codegen/llvm/cg_llvm.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"

TEST(testJIT, testBuildJit)
{
    struct engine *engine = engine_llvm_new(true);
    JIT *jit = build_jit(engine);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testNumber)
{
    char test_code[] = R"(
10
10
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    auto node2 = (struct ast_node *)array_back_ptr(&block->block->nodes);
    result = eval_exp(jit, node2);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testNegNumber)
{
    char test_code[] = R"(
-10
  )";
    struct engine *engine = engine_llvm_new(true);
    JIT *jit = build_jit(engine);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(-10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testRemainderOp)
{
    char test_code[] = R"(
10%3
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(1, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testPositiveNumber)
{
    char test_code[] = R"(
+10
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testChar)
{
    char test_code[] = R"(
'c'
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_CHAR, result.type);
    ASSERT_EQ('c', result.c_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testString)
{
    char test_code[] = R"(
"hello"
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_STRING, result.type);
    ASSERT_STREQ("hello", result.s_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testTypeCast)
{
    char test_code[] = R"(
10 + 10.0
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    //eval_statement(jit, node);
    ASSERT_EQ(TYPE_F64, node->type->type);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testLocalVar)
{
    char test_code[] = R"(
let y=100
y
)";
    struct engine *engine = engine_llvm_new(true);
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block, 0);
    ASSERT_EQ(100, eval_module(jit, block).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testVarString)
{
    char test_code[] = R"(
let y="hello"
y
)";
    struct engine *engine = engine_llvm_new(true);
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block, 0);
    ASSERT_STREQ("hello", eval_module(jit, block).s_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

/*TODO: global var fails randomly
TEST(testJIT, testGlobalVar)
{
    char test_code[] = R"(
let y=100
y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_EQ(100, eval_exp(jit, node1).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testGlobalVarString)
{
    char test_code[] = R"(
let y="hello"
y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
*/
/*
TEST(testJIT, testGlobalVarAssignTwice)
{
    char test_code[] = R"(
y=100
y=200
y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node2 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    eval_statement(jit, node1);
    eval_statement(jit, node2);
    ASSERT_EQ(200, eval_exp(jit, node3).i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
*/
TEST(testJIT, testIdFunc)
{
    char test_code[] = R"(
let id x = x
id 10.0
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node1 = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node2 = (struct ast_node *)array_back_ptr(&block->block->nodes);
    eval_statement(jit, node1);
    auto result = eval_exp(jit, node2);
    ASSERT_EQ(10.0, result.d_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testIdGenericFunc)
{
    char test_code[] = R"(
let id_g x = x
id_g 10.0
id_g 20
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    auto node2 = (struct ast_node *)array_back_ptr(&block->block->nodes);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(10.0, result.d_value);
    result = eval_exp(jit, node2);
    ASSERT_EQ(20, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testSquareFunc)
{
    char test_code[] = R"(
let sq x = x * x
sq 10.0
  )";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(100.0, result.d_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

/*TODO: The following operator override is not supported
TEST(testJIT, testUnaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
y=100
-y
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, 1);
    auto node2 = (struct ast_node *)array_get_ptr(&block->block->nodes, 2);
    eval_statement(jit, node);
    eval_statement(jit, node1);
    ASSERT_EQ(-100, eval_exp(jit, node2).i_value);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testBinaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
binary>10 x y = y < x
z = 100
if z>99 then -z else z
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    ast_node *block = parse_string(env->parser, "test", test_code);
    for (int i = 0; i < 3; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJIT, testUnaryBinaryFunc)
{
    char test_code[] = R"(
(-) x = 0 - x # unary operator overloading
(>)10 x y = y < x # binary operator overloading
z = 100
if z>99 then -z else z
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto end = 3;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = (struct ast_node *)array_get_ptr(&block->block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
    jit_free(jit);
    engine_free(engine);
}*/

TEST(testJIT, testLocalStringFunc)
{
    char test_code[] = R"(
let to_string () = 
  let x = "hello"
  let y = x
  y
to_string()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 1;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node1 = (struct ast_node *)array_get_ptr(&block->block->nodes, end);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

#ifndef _WIN32
//TODO: printf not captured in CaptureStdout()
TEST(testJIT, testPrintfFunc)
{
    char test_code[] = R"(
printf "hello\n"
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto end = 1;
    testing::internal::CaptureStdout();
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto msg = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("hello\n6:int\n", msg.c_str()); //TODO: fix me later
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
/*
TEST(testJIT, testPrintfMoreParamFunc)
{
    char test_code[] = R"(
printf "hello:%d" 1
)";
    struct engine *engine = engine_llvm_new(true);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    testing::internal::CaptureStdout();
    auto end = 1;
    for (int i = 0; i < end; i++) {
        auto node = (struct ast_node *)array_get_ptr(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto msg = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("hello:17:int\n", msg.c_str());
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
*/
#endif
