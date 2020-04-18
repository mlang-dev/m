/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lexer
 */
#include "lexer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testLexer, testEqualOp)
{
    char test_code[] = "==";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("==", token->ident_str->data);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testLEOp)
{
    char test_code[] = "<=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("<=", token->ident_str->data);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testGEOp)
{
    char test_code[] = ">=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ(">=", token->ident_str->data);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testNEOp)
{
    char test_code[] = "!=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("!=", token->ident_str->data);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testInt)
{
    char test_code[] = "30";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_INT, token->type);
    ASSERT_EQ(30, token->int_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testDoubleWithDotZero)
{
    char test_code[] = "30.0";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(30.0, token->double_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testDoubleWithDot)
{
    char test_code[] = "30.";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(30.0, token->double_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testDoubleWithZeroDot)
{
    char test_code[] = "0.5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(0.5, token->double_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testDoubleWithDotNoLeadingZero)
{
    char test_code[] = ".5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(0.5, token->double_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testDouble)
{
    char test_code[] = "30.5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(30.5, token->double_val);
    destroy_tokenizer(tokenizer);
}
