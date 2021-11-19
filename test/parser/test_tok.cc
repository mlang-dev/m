/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/tok.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(testTok, testTokenEmptyString)
{
    symbols_init();
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, "");
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    symbols_deinit();
}

TEST(testTok, testSkipComment)
{
    symbols_init();
    char test_code[] = R"(
#123

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    symbols_deinit();
 }

TEST(testTok, testTokenChar)
{
    symbols_init();
    char test_code[] = R"(
'234'

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(CHAR_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    symbols_deinit();
}

TEST(testTok, testTokenString)
{
    symbols_init();
    char test_code[] = R"(
"234"

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(STRING_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    symbols_deinit();
 }

TEST(testTok, testTokenNumInt)
{
    symbols_init();
    char test_code[] = R"(
234

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(testTok, testTokenNumFloat)
{
    symbols_init();
    char test_code[] = R"(
23.

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
}

TEST(testTok, testTokenNumFloat2)
{
    symbols_init();
    char test_code[] = R"(
2.3

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(testTok, testTokenNumFloat3)
{
    symbols_init();
    char test_code[] = R"(
.23

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(testTok, testTokenID)
{
    symbols_init();
    char test_code[] = R"(
_abc123

)";
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(IDENT_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(8, tok.loc.end);
    symbols_deinit();
 }

TEST(testTok, testTokenMixedNumAndID)
{
    symbols_init();
    char test_code[] = R"(
123abc

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    //NUM
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);

    //IDENT
    get_tok(&lexer, &tok);
    ASSERT_EQ(IDENT_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.loc.row);
    ASSERT_EQ(4, tok.loc.col);
    ASSERT_EQ(4, tok.loc.start);
    ASSERT_EQ(7, tok.loc.end);
    symbols_deinit();
}
