/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "test.h"
#include <stdio.h>


TEST(test_lexer, empty_string)
{
    symbols_init();
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, "");
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    lexer_deinit();
    symbols_deinit();
}

TEST(test_lexer, skip_comment)
{
    symbols_init();
    char test_code[] = 
"\n"
"#123\n"
"\n";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_char)
{
    symbols_init();
    char test_code[] = 
"\n"
"'234'\n"
"\n";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_CHAR, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
}

TEST(test_lexer, token_string)
{
    symbols_init();
    char test_code[] = 
"\n"
"\"234\"\n"
"\n";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_STRING, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_num_int)
{
    symbols_init();
    char test_code[] = 
"\n"
"234\n"
"\n";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_INT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_num_float)
{
    symbols_init();
    char test_code[] = "\n23.";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_FLOAT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
}

TEST(test_lexer, token_num_float2)
{
    symbols_init();
    char test_code[] = "\n2.3";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_FLOAT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_num_float3)
{
    symbols_init();
    char test_code[] = "\n.23";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_FLOAT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_id)
{
    symbols_init();
    char test_code[] = "\n_abc123";
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_IDENT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(8, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
 }

TEST(test_lexer, token_num_id)
{
    symbols_init();
    char test_code[] = "\n123abc";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    //NUM
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_INT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);

    //IDENT
    get_tok(&lexer, &tok);
    ASSERT_EQ(TOKEN_IDENT, tok.tok_type);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(4, tok.loc.col);
    ASSERT_EQ(4, tok.loc.start);
    ASSERT_EQ(7, tok.loc.end);
    lexer_deinit();
    symbols_deinit();
}

int test_lexer()
{
    UNITY_BEGIN();
    RUN_TEST(test_lexer_empty_string);
    RUN_TEST(test_lexer_skip_comment);
    RUN_TEST(test_lexer_token_char);
    RUN_TEST(test_lexer_token_string);
    RUN_TEST(test_lexer_token_id);
    RUN_TEST(test_lexer_token_num_float);
    RUN_TEST(test_lexer_token_num_float2);
    RUN_TEST(test_lexer_token_num_float3);
    RUN_TEST(test_lexer_token_num_id);
    RUN_TEST(test_lexer_token_num_int);
    return UNITY_END();
}
