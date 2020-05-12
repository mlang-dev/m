/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "analyzer.h"
#include "codegen.h"
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzer, testIntVariable)
{
    char test_code[] = "x = 11";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_INT, node->init_value->type->type);
    ASSERT_EQ(KIND_VAR, node->base.type->kind);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_DOUBLE, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testBoolVariable)
{
    char test_code[] = "x = true";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(TYPE_BOOL, node->init_value->type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testCallNode)
{
    char test_code[] = "print 10";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(call_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_EQ(CALL_NODE, node->base.node_type);
    if (node->base.type->type != TYPE_INT)
        assert(false);
    ASSERT_EQ(TYPE_INT, node->base.type->type);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(0, node->base.type);
    env_free(menv);
}

TEST(testAnalyzer, testGreaterThan)
{
    char test_code[] = R"(
  11>10
  )";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    analyze(menv->type_env, (exp_node*)block);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    string type_str = to_string(node->type);
    ASSERT_STREQ("bool", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testIdentityFunc)
{
    reset_id_name("a");
    char test_code[] = "id x = x";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("id", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = *(type_exp**)array_back(&var->args);
    auto from_type = *(type_exp**)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("a -> a", string_get(&type_str));    
    env_free(menv);
}

TEST(testAnalyzer, testIntIntFunc)
{
    char test_code[] = "f x = x + 10";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = *(type_exp**)array_back(&var->args);
    auto from_type = *(type_exp**)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testDoubleDoubleFunc)
{
    char test_code[] = "f x = x + 10.0";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = *(type_exp**)array_back(&var->args);
    auto from_type = *(type_exp**)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double -> double", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testBoolFunc)
{
    char test_code[] = "f x = !x";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool -> bool", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testMultiParamFunc)
{
    char test_code[] = "avg x y = (x + y) / 2";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("avg", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(3, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int * int -> int", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testRecursiveFunc)
{
    char test_code[] = R"(
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)
)";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("factorial", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    env_free(menv);
}

TEST(testAnalyzer, testForLoopFunc)
{
    char test_code[] = R"(
# using for loop
loopprint n = 
  for i in 0..n
    print i
)";
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> ()", string_get(&type_str));
    for_node* forn = *(for_node**)array_front(&node->body->nodes);
    ASSERT_EQ(TYPE_INT, get_type(forn->step->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->start->type));
    ASSERT_EQ(TYPE_BOOL, get_type(forn->end->type));
    ASSERT_EQ(TYPE_INT, get_type(forn->body->type));
    env_free(menv);
}

TEST(testAnalyzer, testLocalVariableFunc)
{
    char test_code[] = R"(
# using for loop
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
)";
    reset_id_name("a");
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    type_env* env = menv->type_env;
    type_exp* sqrt_type = retrieve(env, "sqrt");
    string hello = to_string(sqrt_type);
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("distance", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_EQ(TYPE_FUNCTION, var->base.type);
    ASSERT_EQ(5, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double * double * double * double -> double", string_get(&type_str));
    env_free(menv);
}
