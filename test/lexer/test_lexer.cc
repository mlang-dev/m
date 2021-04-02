/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lexer
 */
#include "lexer/lexer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testLexer, testUnderlineName)
{
    char test_code[] = "__name";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("__name", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testExternToken)
{
    char test_code[] = "extern";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_EXTERN, token->token_type);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testTypeToken)
{
    char test_code[] = "type";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_TYPE, token->token_type);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testIdentToken)
{
    char test_code[] = "x.y";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("x.y", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testEqualOp)
{
    char test_code[] = "==";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("==", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testLEOp)
{
    char test_code[] = "<=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("<=", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testGEOp)
{
    char test_code[] = ">=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ(">=", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testNEOp)
{
    char test_code[] = "!=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_STREQ("!=", string_get(token->str_val));
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

TEST(testLexer, testRange)
{
    char test_code[] = "2..10";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_INT, token->type);
    ASSERT_EQ(2, token->int_val);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_INT, token->type);
    ASSERT_EQ(10, token->int_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testTrueBool)
{
    char test_code[] = "true";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_TRUE, token->token_type);
    ASSERT_EQ(TYPE_BOOL, token->type);
    ASSERT_EQ(1, token->int_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testFalseBool)
{
    char test_code[] = "false";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FALSE, token->token_type);
    ASSERT_EQ(TYPE_BOOL, token->type);
    ASSERT_EQ(0, token->int_val);
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

TEST(testLexer, testMoreDouble)
{
    char test_code[] = "30.12";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_NUM, token->token_type);
    ASSERT_EQ(TYPE_DOUBLE, token->type);
    ASSERT_EQ(30.12, token->double_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testVariadic)
{
    char test_code[] = "...";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_VARIADIC, token->token_type);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testCharLiteral)
{
    char test_code[] = R"('c')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testMultiCharLiteral)
{
    char test_code[] = R"('c' 'd')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('d', token->char_val);
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testCharLiteralInvalidNumberOfChars)
{
    testing::internal::CaptureStderr();
    char test_code[] = R"('cd')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(0, token);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: only one char allowed in character literal\n", error.c_str());
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testCharLiteralInvalidEmptyChar)
{
    testing::internal::CaptureStderr();
    char test_code[] = R"('')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(0, token);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: empty char is not allowed in character literal\n", error.c_str());
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testStringLiteral)
{
    char test_code[] = R"("hello world!")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello world!", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testStringLiteralWithNewLine)
{
    char test_code[] = R"("hello\n")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello\n", string_get(token->str_val));
    ASSERT_EQ(6, strlen(string_get(token->str_val)));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testStringLiteralMulti)
{
    char test_code[] = R"("hello" "world")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("world", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}

TEST(testLexer, testIdWithStringLiteralMulti)
{
    char test_code[] = R"(printf "%s" "hello")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("printf", string_get(token->str_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("%s", string_get(token->str_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
}
