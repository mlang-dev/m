/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "codegen/env.h"
#include "parser/grammar.h"
#include "parser/parser.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

const char test_grammar[] = R"(
sum         = sum [+-] term     {}
            | term              {}
term        = term [*/%] factor {}
            | factor            {}
factor      = '(' sum ')'       {}
            | [+-] factor       {}
            | power             {}
power       = NUM '^' factor    {}
            | NUM               {}

    )";

TEST(testGParser, testTokenEmptyString)
{
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, "");
    get_tok(parser, &tok);
    ASSERT_EQ(0, tok.tok_type);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testSkipComment)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
#123

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(0, tok.tok_type);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenChar)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
'234'

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->CHAR_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(6, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenString)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
"234"

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->STRING_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(6, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenNumInt)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
234

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(4, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenNumFloat)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
23.

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(4, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenNumFloat2)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
2.3

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(4, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenNumFloat3)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
.23

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->NUM_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(4, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testTokenID)
{
    struct env *env = env_new(false);
    char test_code[] = R"(
_abc

)";

    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, test_code);
    get_tok(parser, &tok);
    ASSERT_EQ(parser->ID_TOKEN, tok.tok_type);
    ASSERT_EQ(2, tok.row_no);
    ASSERT_EQ(1, tok.col_no);
    ASSERT_EQ(1, tok.start_pos);
    ASSERT_EQ(5, tok.end_pos);
    parser_free(parser);
    env_free(env);
}

TEST(testGParser, testArithmeticExp)
{
    struct env *env = env_new(false);
    struct parser *parser = parser_new(test_grammar);
    struct tok tok;
    parser_set_text(parser, "");
    get_tok(parser, &tok);
    ASSERT_EQ(0, tok.tok_type);
    parser_free(parser);
    env_free(env);
}
