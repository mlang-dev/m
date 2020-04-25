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
    auto type = analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("int", string_get(&node->init_value->type.name));
    ASSERT_EQ(KIND_VAR, type->kind);
    auto var = (type_var*)type;
    ASSERT_STREQ("int", string_get(&var->instance->name));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    auto type = analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("double", string_get(&node->init_value->type.name));
    ASSERT_EQ(KIND_VAR, type->kind);
    auto var = (type_var*)type;
    ASSERT_STREQ("double", string_get(&var->instance->name));
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, NULL, NULL, NULL);
    type_env* env = type_env_new();
    auto type = analyze(env, (exp_node*)block);
    auto node = *(var_node**)array_front(&block->nodes);
    ASSERT_EQ(1, array_size(&block->nodes));
    ASSERT_STREQ("x", string_get(&node->var_name));
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(NULL, type);
    type_env_free(env);
    parser_free(parser);
}
