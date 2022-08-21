/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT
 */
#include <stdio.h>

#include "compiler/repl.h"
#include "parser/m_parser.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"

TEST(testJIT, testBuildJit)
{
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testNumber)
{
    char test_code[] = R"(
10
10
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    auto node2 = *(ast_node **)array_back(&block->block->nodes);
    result = eval_exp(jit, node2);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testNegNumber)
{
    char test_code[] = R"(
-10
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(-10, result.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testRemainderOp)
{
    char test_code[] = R"(
10%3
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(1, result.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testPositiveNumber)
{
    char test_code[] = R"(
+10
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_INT, result.type);
    ASSERT_EQ(10, result.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testChar)
{
    char test_code[] = R"(
'c'
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_CHAR, result.type);
    ASSERT_EQ('c', result.c_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testString)
{
    char test_code[] = R"(
"hello"
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(TYPE_STRING, result.type);
    ASSERT_STREQ("hello", result.s_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testTypeError)
{
    char test_code[] = R"(
10 + 10.0
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    eval_statement(jit, node);
    ASSERT_EQ(0, node->type);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVar)
{
    char test_code[] = R"(
y=100
y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_EQ(100, eval_exp(jit, node1).i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVarString)
{
    char test_code[] = R"(
y="hello"
y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    eval_statement(jit, node);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testGlobalVarAssignTwice)
{
    char test_code[] = R"(
y=100
y=200
y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto node2 = *(ast_node **)array_get(&block->block->nodes, 1);
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 2);
    eval_statement(jit, node1);
    eval_statement(jit, node2);
    ASSERT_EQ(200, eval_exp(jit, node3).i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testIdFunc)
{
    char test_code[] = R"(
let id x = x
id 10.0
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node1 = *(ast_node **)array_front(&block->block->nodes);
    auto node2 = *(ast_node **)array_back(&block->block->nodes);
    eval_statement(jit, node1);
    auto result = eval_exp(jit, node2);
    ASSERT_EQ(10.0, result.d_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testIdGenericFunc)
{
    char test_code[] = R"(
let id_g x = x
id_g 10.0
id_g 20
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    auto node2 = *(ast_node **)array_back(&block->block->nodes);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(10.0, result.d_value);
    result = eval_exp(jit, node2);
    ASSERT_EQ(20, result.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testSquareFunc)
{
    char test_code[] = R"(
let sq x = x * x
sq 10.0
  )";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    eval_statement(jit, node);
    auto result = eval_exp(jit, node1);
    ASSERT_EQ(100.0, result.d_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testIfFunc)
{
    char test_code[] = R"(
let if_f x = 
    if x < 10 then x
    else 0
if_f 5
if_f 10
)";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    auto node2 = *(ast_node **)array_get(&block->block->nodes, 2);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    eval_result result2 = eval_exp(jit, node2);
    ASSERT_EQ(5, result1.i_value);
    ASSERT_EQ(0, result2.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testForLoopFunc)
{
    char test_code[] = R"(
let forloop n = 
    for i in 1..n
        i
forloop 4
  )";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_back(&block->block->nodes);
    eval_statement(jit, node);
    eval_result result1 = eval_exp(jit, node1);
    ASSERT_EQ(0, result1.i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}
/*TODO: The following operator override is not supported
TEST(testJIT, testUnaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
y=100
-y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    auto node1 = *(ast_node **)array_get(&block->block->nodes, 1);
    auto node2 = *(ast_node **)array_get(&block->block->nodes, 2);
    eval_statement(jit, node);
    eval_statement(jit, node1);
    ASSERT_EQ(-100, eval_exp(jit, node2).i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testBinaryFunc)
{
    char test_code[] = R"(
unary- x = 0 - x
binary>10 x y = y < x
z = 100
if z>99 then -z else z
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->parser, "test", test_code);
    for (int i = 0; i < 3; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testUnaryBinaryFunc)
{
    char test_code[] = R"(
(-) x = 0 - x # unary operator overloading
(>)10 x y = y < x # binary operator overloading
z = 100
if z>99 then -z else z
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto end = 3;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(-100, eval_exp(jit, node3).i_value);
    jit_free(jit);
    env_free(env);
}*/

TEST(testJIT, testLocalStringFunc)
{
    char test_code[] = R"(
let to_string () = 
  x = "hello"
  y = x
  y
to_string()
)";
    env *env = env_new(false);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 1;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node1 = *(ast_node **)array_get(&block->block->nodes, end);
    ASSERT_STREQ("hello", eval_exp(jit, node1).s_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

#ifndef _WIN32
//TODO: printf not captured in CaptureStdout()
TEST(testJIT, testPrintfFunc)
{
    char test_code[] = R"(
printf "hello\n"
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 1;
    testing::internal::CaptureStdout();
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto msg = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("hello\\n7:int\n", msg.c_str()); //TODO: fix me later
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testPrintfMoreParamFunc)
{
    char test_code[] = R"(
printf "hello:%d" 1
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    testing::internal::CaptureStdout();
    auto end = 1;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto msg = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("hello:17:int\n", msg.c_str());
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}
#endif

TEST(testJIT, testProductType)
{
    char test_code[] = R"(
type Point2D = x:double, y:double
xy:Point2D = Point2D(10.0, 20.0)
xy.x
xy.y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    auto node4 = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(20.0, eval_exp(jit, node4).d_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testProductTypeIntType)
{
    char test_code[] = R"(
type Point2D = x:int, y:int
xy:Point2D = Point2D(10, 20)
xy.x
xy.y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(10, eval_exp(jit, node3).i_value);
    auto node4 = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(20, eval_exp(jit, node4).i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testProductTypeMixedType)
{
    char test_code[] = R"(
type Point2D = x:double, y:int
xy:Point2D = Point2D(10.0, 20)
xy.x
xy.y
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    auto node4 = *(ast_node **)array_get(&block->block->nodes, 3);
    ASSERT_EQ(20, eval_exp(jit, node4).i_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}

TEST(testJIT, testProductTypeMixedTypeLocalVariable)
{
    char test_code[] = R"(
type Point2D = x:double, y:int
let getx()=
    xy:Point2D = Point2D(10.0, 20)
    xy.x
getx()
)";
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    struct ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto end = 2;
    for (int i = 0; i < end; i++) {
        auto node = *(ast_node **)array_get(&block->block->nodes, i);
        eval_statement(jit, node);
    }
    auto node3 = *(ast_node **)array_get(&block->block->nodes, 2);
    ASSERT_EQ(10.0, eval_exp(jit, node3).d_value);
    ast_node_free(block);
    jit_free(jit);
    env_free(env);
}
