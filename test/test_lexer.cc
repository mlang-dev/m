#include "gtest/gtest.h"
#include "lexer.h"
#include "test_util.h"
#include <stdio.h>

TEST(testLexer, testEqualOp){
  char test_code[128] = "==";
  auto tokenizer = create_tokenizer_for_string(test_code);
  auto token = get_token(tokenizer);
  ASSERT_EQ(TOKEN_OP, token.type);
  ASSERT_STREQ("==", token.ident_str->c_str());
  destroy_tokenizer(tokenizer);
}

TEST(testLexer, testLEOp){
  char test_code[128] = "<=";
  auto tokenizer = create_tokenizer_for_string(test_code);
  auto token = get_token(tokenizer);
  ASSERT_EQ(TOKEN_OP, token.type);
  ASSERT_STREQ("<=", token.ident_str->c_str());
  destroy_tokenizer(tokenizer);
}

TEST(testLexer, testGEOp){
  char test_code[128] = ">=";
  auto tokenizer = create_tokenizer_for_string(test_code);
  auto token = get_token(tokenizer);
  ASSERT_EQ(TOKEN_OP, token.type);
  ASSERT_STREQ(">=", token.ident_str->c_str());
  destroy_tokenizer(tokenizer);
}

TEST(testLexer, testNEOp){
  char test_code[128] = "!=";
  auto tokenizer = create_tokenizer_for_string(test_code);
  auto token = get_token(tokenizer);
  ASSERT_EQ(TOKEN_OP, token.type);
  ASSERT_STREQ("!=", token.ident_str->c_str());
  destroy_tokenizer(tokenizer);
}
