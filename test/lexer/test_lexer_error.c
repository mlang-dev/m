/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "sema/frontend.h"
#include "error/error.h"
#include "test.h"
#include <stdio.h>


TEST(test_lexer, string_error_missing_end_quote)
{
    if(TEST_PROTECT()){
        struct frontend *fe = frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("\"");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_NULL, tok->token_type);
        struct error_report *er = get_last_error_report(lexer);
        ASSERT_STREQ("missing end quote for string literal.", er->error_msg);
        ASSERT_EQ(1, er->loc.line);
        ASSERT_EQ(1, er->loc.col);
        lexer_free(lexer);
        frontend_deinit(fe);
        TEST_ABORT();
    }
}

TEST(test_lexer, char_error_missing_end_quote)
{
    if(TEST_PROTECT()){
        struct frontend *fe = frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("'");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_NULL, tok->token_type);
        struct error_report *er = get_last_error_report(lexer);
        ASSERT_STREQ("missing end quote for char literal.", er->error_msg);
        ASSERT_EQ(1, er->loc.line);
        ASSERT_EQ(1, er->loc.col);
        lexer_free(lexer);
        frontend_deinit(fe);
        TEST_ABORT();
    }
}

TEST(test_lexer, char_error_multichar_end_quote)
{
    if(TEST_PROTECT()){
        struct frontend *fe = frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("'abc'");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_NULL, tok->token_type);
        struct error_report *er = get_last_error_report(lexer);
        ASSERT_STREQ("character literal is found to have more than 1 character.", er->error_msg);
        ASSERT_EQ(1, er->loc.line);
        ASSERT_EQ(1, er->loc.col);
        lexer_free(lexer);
        frontend_deinit(fe);
        TEST_ABORT();
    }
}

TEST(test_lexer, indent_level_error)
{
    if(TEST_PROTECT()){
        struct frontend *fe = frontend_init();
        struct token *tok = 0;
        struct lexer *lexer;
        char code[] = "\n\
10\n\
    20\n\
  30\n\
";
        lexer = lexer_new_with_string(code);
        for(int i = 0; i < 6; i++)
            tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_NULL, tok->token_type);
        struct error_report *er = get_last_error_report(lexer);
        ASSERT_STREQ("inconsistent indent level found.", er->error_msg);
        ASSERT_EQ(4, er->loc.line);
        ASSERT_EQ(3, er->loc.col);
        lexer_free(lexer);
        frontend_deinit(fe);
        TEST_ABORT();
    }
}

int test_lexer_error()
{
    UNITY_BEGIN();
    RUN_TEST(test_lexer_string_error_missing_end_quote);
    RUN_TEST(test_lexer_char_error_missing_end_quote);
    RUN_TEST(test_lexer_char_error_multichar_end_quote);
    RUN_TEST(test_lexer_indent_level_error);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
