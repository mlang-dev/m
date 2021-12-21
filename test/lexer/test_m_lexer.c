/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lexer
 */
#include "lexer/lexer.h"
#include "tutil.h"
#include "parser/m_grammar.h"
#include "test.h"
#include <stdio.h>

TEST(test_m_lexer, underscore)
{
    symbols_init();
    char test_code[] = "__name";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("__name", string_get(token->symbol_val));
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, extern_token)
{
    symbols_init();
    char test_code[] = "extern";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_EXTERN, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, type_token)
{
    symbols_init();
    char test_code[] = "type";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_TYPE, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, ident_token)
{
    symbols_init();
    char test_code[] = "x.y";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("x", string_get(token->symbol_val));
    ASSERT_EQ(TOKEN_DOT, get_tok(lexer)->token_type);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("y", string_get(token->symbol_val));
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, op_eq)
{
    symbols_init();
    char test_code[] = "==";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_EQ, token->opcode);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, op_le)
{
    symbols_init();
    char test_code[] = "<=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_LE, token->opcode);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, op_ge)
{
    symbols_init();
    char test_code[] = ">=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_GE, token->opcode);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, op_ne)
{
    symbols_init();
    char test_code[] = "!=";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, token->token_type);
    ASSERT_EQ(OP_NE, token->opcode);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, int_literal)
{
    symbols_init();
    char test_code[] = "30";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(30, token->int_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, range_only)
{
    symbols_init();
    char test_code[] = "..";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, range)
{
    symbols_init();
    char test_code[] = "2..10";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(2, token->int_val);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_RANGE, token->token_type);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, token->token_type);
    ASSERT_EQ(10, token->int_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, true_literal)
{
    symbols_init();
    char test_code[] = "true";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_TRUE, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, false_literal)
{
    symbols_init();
    char test_code[] = "false";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_FALSE, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, double_literal)
{
    symbols_init();
    char test_code[] = "30.0";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.0, token->double_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, double_end_with_dot)
{
//     symbols_init();
//     char test_code[] = "30.";
//     struct lexer *lexer = lexer_new_for_string(test_code);;
//     struct token *token = get_tok(lexer);
//     ASSERT_EQ(TOKEN_FLOAT, token->token_type);
//     ASSERT_EQ(30.0, token->double_val);
//     lexer_free(lexer);
//     symbols_deinit();
// 
}

TEST(test_m_lexer, double_start_with_dot)
{
    symbols_init();
    char test_code[] = ".5";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(0.5, token->double_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, double_more)
{
    symbols_init();
    char test_code[] = "30.12";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_FLOAT, token->token_type);
    ASSERT_EQ(30.12, token->double_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, variadic)
{
    symbols_init();
    char test_code[] = "...";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_VARIADIC, token->token_type);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, char_literal)
{
    symbols_init();
    char test_code[] = "'c'";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, char_multi_literal)
{
    symbols_init();
    char test_code[] = "'c' 'd'";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('c', token->char_val);
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_CHAR, token->token_type);
    ASSERT_EQ('d', token->char_val);
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, char_invalid_number)
{
    // symbols_init();
    // char test_code[] = "'cd'";
    // struct lexer *lexer = lexer_new_for_string(test_code);;
    // struct token *token = get_tok(lexer);
    // ASSERT_EQ(0, token);
    // //auto error = testing::internal::GetCapturedStderr();
    // //ASSERT_STREQ("error: :1:1: only one char allowed in character literal\n", error.c_str());
    // lexer_free(lexer);
    // symbols_deinit();
}

TEST(test_m_lexer, char_invalid_char)
{
    // symbols_init();
    // //testing::internal::CaptureStderr();
    // char test_code[] = "''";
    // struct lexer *lexer = lexer_new_for_string(test_code);;
    // struct token *token = get_tok(lexer);
    // ASSERT_EQ(0, token);
    // //auto error = testing::internal::GetCapturedStderr();
    // //ASSERT_STREQ("error: :1:1: empty char is not allowed in character literal\n", error.c_str());
    // lexer_free(lexer);
    // symbols_deinit();
}

TEST(test_m_lexer, string_literal)
{
    symbols_init();
    char test_code[] = "\"hello world!\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello world!", string_get(token->str_val));
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, string_literal_with_newline)
{
    symbols_init();
    char test_code[] = "\"hello\\n\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello\\n", string_get(token->str_val));
    ASSERT_EQ(7, strlen(string_get(token->str_val)));
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, string_multi)
{
    symbols_init();
    char test_code[] = "\"hello\" \"world\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("world", string_get(token->str_val));
    lexer_free(lexer);
    symbols_deinit();
}

TEST(test_m_lexer, string_with_id)
{
    symbols_init();
    char test_code[] = "printf \"%s\" \"hello\"";
    struct lexer *lexer = lexer_new_for_string(test_code);;
    struct token *token = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, token->token_type);
    ASSERT_STREQ("printf", string_get(token->symbol_val));
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("%s", string_get(token->str_val));
    token = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, token->token_type);
    ASSERT_STREQ("hello", string_get(token->str_val));
    lexer_free(lexer);
    symbols_deinit();
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
    return UNITY_END();
}
