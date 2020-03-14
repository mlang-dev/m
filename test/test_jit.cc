#include "gtest/gtest.h"
#include "parser.h"
#include "repl.h"
#include "test_util.h"
#include <stdio.h>

TEST(testJIT, testIdFunc){
  char test_code[128] = R"(
  f x = x
  f 10
  )";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  auto result = eval_exp(jit, block->nodes[1]);
  ASSERT_EQ(10.0, result);
  destroy_parser(parser);
}

TEST(testJIT, testSqrtFunc){
  char test_code[128] = R"(
  f x = x * x
  f 10
  )";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  auto result = eval_exp(jit, block->nodes[1]);
  ASSERT_EQ(100.0, result);
  destroy_parser(parser);
}

TEST(testJIT, testIfFunc){
  char test_code[128] = R"(
  f x = 
    if x < 10 then x
    else 0
  f 5
  f 10
  )";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  ASSERT_EQ(5.0, eval_exp(jit, block->nodes[1]));
  ASSERT_EQ(0, eval_exp(jit, block->nodes[2]));
  destroy_parser(parser);
}

TEST(testJIT, testGloVarFunc){
  char test_code[128] = R"(
y=100
y
)";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  ASSERT_EQ(100.0, eval_exp(jit, block->nodes[1]));
  destroy_parser(parser);
}

TEST(testJIT, testUnaryFunc){
  char test_code[128] = R"(
unary- x = 0 - x
y=100
-y
)";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  eval_statement(jit, block->nodes[1]);
  ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[2]));
  destroy_parser(parser);
}

TEST(testJIT, testBinaryFunc){
  char test_code[128] = R"(
unary- x = 0 - x
binary>10 x y = y < x
z = 100
if z>99 then -z else z
)";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  for(int i=0;i<3;i++)
    eval_statement(jit, block->nodes[i]);
  ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[3]));
  destroy_parser(parser);
}

// TEST(testJIT, testUnaryBinaryFunc){
//   char test_code[128] = R"(
// (-) x = 0 - x # unary operator overloading
// (>)10 x y = y < x # binary operator overloading
// z = 100
// if z>99 then -z else z
// )";
//   auto parser = create_parser_for_string(test_code);
//   JIT* jit = build_jit(parser);
//   block_node * block = parse_block(parser, nullptr);
//   for(int i=0;i<3;i++)
//     eval_statement(jit, block->nodes[i]);
//   ASSERT_EQ(-100.0, eval_exp(jit, block->nodes[3]));
//   destroy_parser(parser);
// }
