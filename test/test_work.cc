#include "gtest/gtest.h"
#include "parser.h"
#include "repl.h"
#include "test_util.h"
#include <stdio.h>
#include "astdump.h"

TEST(testWork, testBinaryFunc0){
  char test_code[128] = R"(
binary>10 x y = y < x
f z = if z > 99 then z-100 else z
f 101
)";
  auto parser = create_parser_for_string(test_code);
  JIT* jit = build_jit(parser);
  block_node * block = parse_block(parser, nullptr);
  eval_statement(jit, block->nodes[0]);
  eval_statement(jit, block->nodes[1]);
  ASSERT_EQ(1.0, eval_exp(jit, block->nodes[2]));
  destroy_parser(parser);
}
