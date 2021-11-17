/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "codegen/env.h"
#include "parser/grammar.h"
#include "parser/tok.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(testTok, testTokenEmptyString)
{
    struct env *env = env_new(false);
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, "");
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    env_free(env);
}

TEST(testTok, testSkipComment)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
#123

)";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    env_free(env);
}

TEST(testTok, testTokenChar)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenString)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenNumInt)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenNumFloat)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenNumFloat2)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenNumFloat3)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenID)
{
    struct env *env = env_new(false);
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
    env_free(env);
}

TEST(testTok, testTokenMixedNumAndID)
{
    struct env *env = env_new(false);
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
    env_free(env);
}
