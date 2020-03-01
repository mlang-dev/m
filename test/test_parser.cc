#include "gtest/gtest.h"
#include "parser.h"
#include <stdio.h>


FILE* _open_file(const char* file_name){
  char* file_content = (char*)file_name;
  return fmemopen(file_content, strlen(file_content), "r");
}

parser* _create_parser_for_string(char* str){
  FILE* file = fmemopen(str, strlen(str), "r");
  return create_parser(str, false, _open_file);
}

TEST(testParser, testBlockVariable){
  char test_code[128] = "x = 11";
  auto parser = _create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (var_node*)block->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("x", node->var_name.c_str());
  ASSERT_EQ(VAR_NODE, node->base.type);
  ASSERT_EQ(NUMBER_NODE, node->init_value->type);
  destroy_parser(parser);
}

TEST(testParser, testBlockIdFunction){
  char test_code[128] = "f x = x";
  auto parser = _create_parser_for_string(test_code);
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
  auto parser = _create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  auto node = (function_node*)block->nodes[0];
  auto body = node->body->nodes[0];
  ASSERT_EQ(1, block->nodes.size());
  ASSERT_STREQ("f", node->prototype->name.c_str());
  ASSERT_EQ(BINARY_NODE, body->type);
  destroy_parser(parser);
}