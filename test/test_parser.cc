#include "gtest/gtest.h"
#include "parser.h"
#include "test_util.h"
#include <stdio.h>


TEST(testParser, testBlockVariable){
  char test_code[128] = "x = 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.type);
  ASSERT_EQ(NUMBER_NODE, node->init_value->type);
  destroy_parser(parser);
}

TEST(testParser, testBlockVariableNameWithUnderlyingLine){
  char test_code[128] = "m_x = 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("m_x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.type);
  ASSERT_EQ(NUMBER_NODE, node->init_value->type);
  destroy_parser(parser);
}

TEST(testParser, testBlockIdFunction){
  char test_code[128] = "f x = x";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f", node->prototype->name.c_str());
  ASSERT_EQ(IDENT_NODE, body->type);
  destroy_parser(parser);
}

TEST(testParser, testBlockBinaryFunction){
  char test_code[128] = "f x = x * x";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f", node->prototype->name.c_str());
  ASSERT_EQ(BINARY_NODE, body->type);
  destroy_parser(parser);
}

TEST(testParser, testBlockBinaryFunctionName){
  char test_code[128] = "f_sq x = x * x";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f_sq", node->prototype->name.c_str());
  ASSERT_EQ(BINARY_NODE, body->type);
  destroy_parser(parser);
}

TEST(testParser, testFacIfCondition){
  char test_code[128] = R"(fac n = 
    if n< 2 n
    else n * fac (n-1))";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("fac", node->prototype->name.c_str());
  ASSERT_EQ(CONDITION_NODE, body->type);
  destroy_parser(parser);
}

TEST(testParser, testForLoop){
  char test_code[128] = R"(loopprint n = 
  for i in 0..n
    print i
  )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("loopprint", node->prototype->name.c_str());
  ASSERT_EQ(FOR_NODE, body->type);
  destroy_parser(parser);
}

TEST(testParser, testVariableInFunction){
  char test_code[128] = R"(distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
  )";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("distance", node->prototype->name.c_str());
  ASSERT_EQ(VAR_NODE, body->type);
  destroy_parser(parser);
}

