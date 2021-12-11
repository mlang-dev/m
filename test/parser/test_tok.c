/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/tok.h"
#include "test.h"
#include <stdio.h>


TEST(test_tok, empty_string)
{
    symbols_init();
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, "");
    get_tok(&lexer, &tok);
    ASSERT_EQ(0, tok.tok_type_name);
    symbols_deinit();
}

TEST(test_tok, skip_comment)
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
    ASSERT_EQ(0, tok.tok_type_name);
    symbols_deinit();
 }

TEST(test_tok, token_char)
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
    ASSERT_EQ(CHAR_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    symbols_deinit();
}

TEST(test_tok, token_string)
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
    ASSERT_EQ(STRING_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(6, tok.loc.end);
    symbols_deinit();
 }

TEST(test_tok, token_num_int)
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
    ASSERT_EQ(NUM_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(test_tok, token_num_float)
{
    symbols_init();
    char test_code[] = "\n23.";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
}

TEST(test_tok, token_num_float2)
{
    symbols_init();
    char test_code[] = "\n2.3";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(test_tok, token_num_float3)
{
    symbols_init();
    char test_code[] = "\n.23";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);
    symbols_deinit();
 }

TEST(test_tok, token_id)
{
    symbols_init();
    char test_code[] = "\n_abc123";
    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    get_tok(&lexer, &tok);
    ASSERT_EQ(IDENT_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(8, tok.loc.end);
    symbols_deinit();
 }

TEST(test_tok, token_num_id)
{
    symbols_init();
    char test_code[] = "\n123abc";

    struct tok tok;
    struct lexer lexer;
    lexer_init(&lexer, test_code);
    //NUM
    get_tok(&lexer, &tok);
    ASSERT_EQ(NUM_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(1, tok.loc.col);
    ASSERT_EQ(1, tok.loc.start);
    ASSERT_EQ(4, tok.loc.end);

    //IDENT
    get_tok(&lexer, &tok);
    ASSERT_EQ(IDENT_TOKEN, tok.tok_type_name);
    ASSERT_EQ(2, tok.loc.line);
    ASSERT_EQ(4, tok.loc.col);
    ASSERT_EQ(4, tok.loc.start);
    ASSERT_EQ(7, tok.loc.end);
    symbols_deinit();
}

int test_tok()
{
    UNITY_BEGIN();
    RUN_TEST(test_tok_empty_string);
    RUN_TEST(test_tok_skip_comment);
    RUN_TEST(test_tok_token_char);
    RUN_TEST(test_tok_token_string);
    RUN_TEST(test_tok_token_id);
    RUN_TEST(test_tok_token_num_float);
    RUN_TEST(test_tok_token_num_float2);
    RUN_TEST(test_tok_token_num_float3);
    RUN_TEST(test_tok_token_num_id);
    RUN_TEST(test_tok_token_num_int);
    return UNITY_END();
}
