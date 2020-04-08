#include "gtest/gtest.h"
#include "parser.h"
#include "analyzer.h"
#include "tutil.h"
#include <stdio.h>

TEST(testAnalyzer, testIntVariable){
  char test_code[] = "x = 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  type_env* env = create_type_env();
  auto type = analyze(env, block)[0];
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.node_type);
  ASSERT_STREQ("int", node->init_value->type.name.c_str());
  ASSERT_EQ(KIND_VAR, type->kind);
  auto var = (type_var*)type;
  ASSERT_STREQ("int", var->instance->name.c_str());
  destroy_type_env(env);
  destroy_parser(parser);
}

TEST(testAnalyzer, testDoubleVariable){
  char test_code[] = "x = 11.0";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  type_env* env = create_type_env();
  auto type = analyze(env, block)[0];
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.node_type);
  ASSERT_STREQ("double", node->init_value->type.name.c_str());
  ASSERT_EQ(KIND_VAR, type->kind);
  auto var = (type_var*)type;
  ASSERT_STREQ("double", var->instance->name.c_str());
  destroy_type_env(env);
  destroy_parser(parser);
}

TEST(testAnalyzer, testDoubleIntLiteralError){
  char test_code[] = "x = 11.0 + 10";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  type_env* env = create_type_env();
  auto type = analyze(env, block)[0];
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.node_type);
  ASSERT_EQ(nullptr, type);
  destroy_type_env(env);
  destroy_parser(parser);
}

