/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "analyzer.h"
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzer, testIntVariable)
{
    char test_code[] = "x = 11";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("int", string_get(&node->init_value->type->name));
    ASSERT_EQ(KIND_VAR, node->base.type->kind);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("double", string_get(&node->init_value->type->name));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}


TEST(testAnalyzer, testBoolVariable)
{
    char test_code[] = "x = true";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("bool", string_get(&node->init_value->type->name));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(NULL, node->base.type);
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testIdentityFunc)
{
    reset_id_name("a");
    char test_code[] = "id x = x";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("id", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = (type_exp*)array_back(&var->args);
    auto from_type = (type_exp*)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("a -> a", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testIntIntFunc)
{
    char test_code[] = "f x = x + 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = (type_exp*)array_back(&var->args);
    auto from_type = (type_exp*)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}


TEST(testAnalyzer, testDoubleDoubleFunc)
{
    char test_code[] = "f x = x + 10.0";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    auto result_type = (type_exp*)array_back(&var->args);
    auto from_type = (type_exp*)array_front(&var->args);
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("double -> double", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testBoolFunc)
{
    char test_code[] = "f x = !x";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("f", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("bool -> bool", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}


TEST(testAnalyzer, testMultiParamFunc)
{
    char test_code[] = "avg x y = (x + y) / 2";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("avg", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(3, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int * int -> int", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testRecursiveFunc)
{
    char test_code[] = R"(
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)
)";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("factorial", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> int", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}


TEST(testAnalyzer, testForLoopFunc)
{
    char test_code[] = R"(
# using for loop
loopprint n = 
  for i in 0..n
    print i
)";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    analyze(env, (exp_node*)block);
    auto node = *(function_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("loopprint", string_get(&node->prototype->name));
    ASSERT_EQ(FUNCTION_NODE, node->base.node_type);
    auto var = (type_oper*)node->base.type;
    ASSERT_STREQ("->", string_get(&var->base.name));
    ASSERT_EQ(2, array_size(&var->args));
    string type_str = to_string(node->base.type);
    ASSERT_STREQ("int -> ()", string_get(&type_str));
    type_env_free(env);
    parser_free(parser);
}
