/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lexer
 */
#include "lexer/m_lexer.h"
#include "tutil.h"
#include "parser/m_grammar.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testLexer, testUnderlineName)
{
    symbols_init();
    char test_code[] = "__name";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("__name", string_get(token->symbol_val));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testExternToken)
{
    symbols_init();
    char test_code[] = "extern";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_EXTERN, token->token_type);
    ASSERT_EQ(to_symbol("extern"), token->symbol_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testTypeToken)
{
    symbols_init();
    char test_code[] = "type";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_TYPE, token->token_type);
    ASSERT_EQ(to_symbol("type"), token->symbol_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testIdentToken)
{
    symbols_init();
    char test_code[] = "x.y";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("x.y", string_get(token->symbol_val));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testEqualOp)
{
    symbols_init();
    char test_code[] = "==";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_EQ, token->opcode);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testLEOp)
{
    symbols_init();
    char test_code[] = "<=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_LE, token->opcode);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testGEOp)
{
    symbols_init();
    char test_code[] = ">=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_GE, token->opcode);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testNEOp)
{
    symbols_init();
    char test_code[] = "!=";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_NE, token->opcode);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testInt)
{
    symbols_init();
    char test_code[] = "30";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(30, token->int_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testRange)
{
    symbols_init();
    char test_code[] = "2..10";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(2, token->int_val);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    ASSERT_EQ(to_symbol(".."), token->symbol_val);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(10, token->int_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testTrueBool)
{
    symbols_init();
    char test_code[] = "true";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_TRUE, token->token_type);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testFalseBool)
{
    symbols_init();
    char test_code[] = "false";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FALSE, token->token_type);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testDoubleWithDotZero)
{
    symbols_init();
    char test_code[] = "30.0";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.0, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testDoubleWithDot)
{
    symbols_init();
    char test_code[] = "30.";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.0, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testDoubleWithZeroDot)
{
    symbols_init();
    char test_code[] = "0.5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(0.5, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testDoubleWithDotNoLeadingZero)
{
    symbols_init();
    char test_code[] = ".5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(0.5, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testDouble)
{
    symbols_init();
    char test_code[] = "30.5";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.5, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testMoreDouble)
{
    symbols_init();
    char test_code[] = "30.12";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.12, token->double_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testVariadic)
{
    symbols_init();
    char test_code[] = "...";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_VARIADIC, token->token_type);
    ASSERT_EQ(to_symbol("..."), token->symbol_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testRangeOnly)
{
    symbols_init();
    char test_code[] = "..";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testCharLiteral)
{
    symbols_init();
    char test_code[] = R"('c')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testMultiCharLiteral)
{
    symbols_init();
    char test_code[] = R"('c' 'd')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('d', token->char_val);
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testCharLiteralInvalidNumberOfChars)
{
    symbols_init();
    testing::internal::CaptureStderr();
    char test_code[] = R"('cd')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(0, token);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: only one char allowed in character literal\n", error.c_str());
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testCharLiteralInvalidEmptyChar)
{
    symbols_init();
    testing::internal::CaptureStderr();
    char test_code[] = R"('')";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(0, token);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: empty char is not allowed in character literal\n", error.c_str());
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testStringLiteral)
{
    symbols_init();
    char test_code[] = R"("hello world!")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello world!", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testStringLiteralWithNewLine)
{
    symbols_init();
    char test_code[] = R"("hello\n")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello\n", string_get(token->str_val));
    ASSERT_EQ(6, strlen(string_get(token->str_val)));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testStringLiteralMulti)
{
    symbols_init();
    char test_code[] = R"("hello" "world")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("world", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}

TEST(testLexer, testIdWithStringLiteralMulti)
{
    symbols_init();
    char test_code[] = R"(printf "%s" "hello")";
    auto tokenizer = create_tokenizer_for_string(test_code);
    auto token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("printf", string_get(token->symbol_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("%s", string_get(token->str_val));
    token = get_token(tokenizer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    destroy_tokenizer(tokenizer);
    symbols_deinit();
}
