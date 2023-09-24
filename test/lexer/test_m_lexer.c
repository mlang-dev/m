/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lexer
 */
#include "lexer/lexer.h"
#include "sema/frontend.h"
#include "tutil.h"
#include "test.h"
#include <stdio.h>

TEST(test_m_lexer, underscore)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "__name";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("__name", string_get(token->symbol_val));
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, extern_token)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "extern";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_EXTERN, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, type_token)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "type";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_TYPE, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, ident_token)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x.y";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("x", string_get(token->symbol_val));
    ASSERT_EQ(OP_DOT, get_tok(lexer)->opcode);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("y", string_get(token->symbol_val));
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, op_eq)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "==";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_EQ, token->opcode);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, op_le)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "<=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_LE, token->opcode);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, op_ge)
{
    struct frontend *fe = frontend_init();
    char test_code[] = ">=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_GE, token->opcode);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, op_ne)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "!=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_NE, token->opcode);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, int_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "30";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_INT, token->token_type);
    ASSERT_EQ(30, token->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, range_only)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "..";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, range)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "2..10";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_INT, token->token_type);
    ASSERT_EQ(2, token->int_val);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_INT, token->token_type);
    ASSERT_EQ(10, token->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, true_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "True";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_TRUE, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, false_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "False";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_FALSE, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, double_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "30.0";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_FLOAT, token->token_type);
    ASSERT_EQ(30.0, token->double_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, double_end_with_dot)
{
//     struct frontend *fe = frontend_init();
//     char test_code[] = "30.";
//     struct lexer *lexer = lexer_new_for_string(test_code);;
//     struct token *token = get_tok(lexer);
//     ASSERT_EQ(TOKEN_LITERAL_FLOAT, token->token_type);
//     ASSERT_EQ(30.0, token->double_val);
//     lexer_free(lexer);
//     frontend_deinit(fe);
// 
}

TEST(test_m_lexer, double_start_with_dot)
{
    struct frontend *fe = frontend_init();
    char test_code[] = ".5";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_FLOAT, token->token_type);
    ASSERT_EQ(0.5, token->double_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, double_more)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "30.12";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_FLOAT, token->token_type);
    ASSERT_EQ(30.12, token->double_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, variadic)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "...";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_VARIADIC, token->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, char_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "'c'";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_CHAR, token->token_type);
    ASSERT_EQ('c', token->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, char_multi_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "'c' 'd'";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_CHAR, token->token_type);
    ASSERT_EQ('c', token->int_val);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_CHAR, token->token_type);
    ASSERT_EQ('d', token->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, char_invalid_number)
{
    // struct frontend *fe = frontend_init();
    // char test_code[] = "'cd'";
    // struct lexer *lexer = lexer_new_for_string(test_code);;
    // struct token *token = get_tok(lexer);
    // ASSERT_EQ(0, token);
    // //auto error = testing::internal::GetCapturedStderr();
    // //ASSERT_STREQ("error: :1:1: only one char allowed in character literal\n", error.c_str());
    // lexer_free(lexer);
    // frontend_deinit(fe);
}

TEST(test_m_lexer, char_invalid_char)
{
    // struct frontend *fe = frontend_init();
    // //testing::internal::CaptureStderr();
    // char test_code[] = "''";
    // struct lexer *lexer = lexer_new_for_string(test_code);;
    // struct token *token = get_tok(lexer);
    // ASSERT_EQ(0, token);
    // //auto error = testing::internal::GetCapturedStderr();
    // //ASSERT_STREQ("error: :1:1: empty char is not allowed in character literal\n", error.c_str());
    // lexer_free(lexer);
    // frontend_deinit(fe);
}

TEST(test_m_lexer, string_literal)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\"hello world!\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("hello world!", token->str_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, string_literal_with_newline)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\"hello\\n\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("hello\n", token->str_val);
    ASSERT_EQ(6, strlen(token->str_val));
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, string_multi)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\"hello\" \"world\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("hello", token->str_val);
    tok_clean(token);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("world", token->str_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_m_lexer, string_with_id)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "printf \"%s\" \"hello\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("printf", string_get(token->symbol_val));
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("%s", token->str_val);
    tok_clean(token);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_LITERAL_STRING, token->token_type);
    ASSERT_STREQ("hello", token->str_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

int test_m_lexer()
{
    UNITY_BEGIN();
    RUN_TEST(test_m_lexer_underscore);
    RUN_TEST(test_m_lexer_extern_token);
    RUN_TEST(test_m_lexer_type_token);
    RUN_TEST(test_m_lexer_ident_token);
    RUN_TEST(test_m_lexer_op_eq);
    RUN_TEST(test_m_lexer_op_le);
    RUN_TEST(test_m_lexer_op_ge);
    RUN_TEST(test_m_lexer_op_ne);
    RUN_TEST(test_m_lexer_range_only);
    RUN_TEST(test_m_lexer_range);
    RUN_TEST(test_m_lexer_int_literal);
    RUN_TEST(test_m_lexer_true_literal);
    RUN_TEST(test_m_lexer_false_literal);
    RUN_TEST(test_m_lexer_double_literal);
    RUN_TEST(test_m_lexer_double_end_with_dot);
    RUN_TEST(test_m_lexer_double_start_with_dot);
    RUN_TEST(test_m_lexer_double_more);
    RUN_TEST(test_m_lexer_variadic);
    RUN_TEST(test_m_lexer_char_literal);
    RUN_TEST(test_m_lexer_char_multi_literal);
    RUN_TEST(test_m_lexer_char_invalid_number);
    RUN_TEST(test_m_lexer_char_invalid_char);
    RUN_TEST(test_m_lexer_string_literal);
    RUN_TEST(test_m_lexer_string_literal_with_newline);
    RUN_TEST(test_m_lexer_string_multi);
    RUN_TEST(test_m_lexer_string_with_id);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
