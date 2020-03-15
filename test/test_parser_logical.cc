#include "gtest/gtest.h"
#include "parser.h"
#include "test_util.h"
#include <stdio.h>

TEST(testParserLogical, testComparisonLessThan){
  char test_code[128] = "10 < 11";
  auto parser = create_parser_for_string(test_code);
  block_node * block = parse_block(parser, nullptr);
  ASSERT_EQ(BINARY_NODE, block->nodes[0]->type);
  auto node = (binary_node*)block->nodes[0];
  ASSERT_EQ('<', node->op);
  destroy_parser(parser);
}
