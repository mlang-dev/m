/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>


TEST(test_lexer, string_error_missing_end_quote)
{
    if(TEST_PROTECT()){
        frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("\"");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_ERROR, tok->token_type);
        ASSERT_STREQ("missing end quote for string literal. location: (1, 1)\n", string_get(tok->str_val));
        lexer_free(lexer);
        frontend_deinit();
        TEST_ABORT();
    }
}

TEST(test_lexer, char_error_missing_end_quote)
{
    if(TEST_PROTECT()){
        frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("'");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_ERROR, tok->token_type);
        ASSERT_STREQ("character is supposed to be 1 char long but got 0 long. location: (1, 1)\n", string_get(tok->str_val));
        lexer_free(lexer);
        frontend_deinit();
        TEST_ABORT();
    }
}

TEST(test_lexer, char_error_multichar_end_quote)
{
    if(TEST_PROTECT()){
        frontend_init();
        struct token *tok;
        struct lexer *lexer;
        lexer = lexer_new_with_string("'abc'");
        tok = get_tok(lexer);
        ASSERT_EQ(TOKEN_ERROR, tok->token_type);
        ASSERT_STREQ("character is supposed to be 1 char long but got 3 long. location: (1, 1)\n", string_get(tok->str_val));
        lexer_free(lexer);
        frontend_deinit();
        TEST_ABORT();
    }
}

int test_lexer_error()
{
    UNITY_BEGIN();
    RUN_TEST(test_lexer_string_error_missing_end_quote);
    RUN_TEST(test_lexer_char_error_missing_end_quote);
    RUN_TEST(test_lexer_char_error_multichar_end_quote);
    return UNITY_END();
}
