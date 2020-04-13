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
    block_node* block = parse_block(parser, nullptr);
    type_env* env = type_env_new();
    auto type = analyze(env, (exp_node*)block);
    auto node = (var_node*)block->nodes[0];
    ASSERT_EQ(1, block->nodes.size());
    ASSERT_STREQ("x", node->var_name.c_str());
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("int", node->init_value->type.name.c_str());
    ASSERT_EQ(KIND_VAR, type->kind);
    auto var = (type_var*)type;
    ASSERT_STREQ("int", var->instance->name.c_str());
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleVariable)
{
    char test_code[] = "x = 11.0";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    type_env* env = type_env_new();
    auto type = analyze(env, (exp_node*)block);
    auto node = (var_node*)block->nodes[0];
    ASSERT_EQ(1, block->nodes.size());
    ASSERT_STREQ("x", node->var_name.c_str());
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_STREQ("double", node->init_value->type.name.c_str());
    ASSERT_EQ(KIND_VAR, type->kind);
    auto var = (type_var*)type;
    ASSERT_STREQ("double", var->instance->name.c_str());
    type_env_free(env);
    parser_free(parser);
}

TEST(testAnalyzer, testDoubleIntLiteralError)
{
    char test_code[] = "x = 11.0 + 10";
    auto parser = create_parser_for_string(test_code);
    block_node* block = parse_block(parser, nullptr);
    type_env* env = type_env_new();
    auto type = analyze(env, (exp_node*)block);
    auto node = (var_node*)block->nodes[0];
    ASSERT_EQ(1, block->nodes.size());
    ASSERT_STREQ("x", node->var_name.c_str());
    ASSERT_EQ(VAR_NODE, node->base.node_type);
    ASSERT_EQ(nullptr, type);
    type_env_free(env);
    parser_free(parser);
}
