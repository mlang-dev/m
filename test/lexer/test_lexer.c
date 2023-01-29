/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/lexer.h"
#include "sema/frontend.h"
#include "test.h"


TEST(test_lexer, empty_string)
{
    struct frontend *fe = frontend_init();
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string("");
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_EOF, tok->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, skip_comment)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\n"
"//123\n"
"/*block comments*/"
"\n";
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    ASSERT_EQ(TOKEN_EOF, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

TEST(test_lexer, skip_comment_in_func)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\n"
"let id x = x //123\n"
"\n";
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    ASSERT_EQ(TOKEN_LET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_OP, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_EOF, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

TEST(test_lexer, token_char)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"'4'\n";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_CHAR, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(3, tok->loc.end);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_char_with_escape)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"'\n'";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_with_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_CHAR, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(3, tok->loc.end);
    ASSERT_EQ('\n', tok->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}


TEST(test_lexer, token_bool_litteral_true)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"true";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_with_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_TRUE, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_bool_litteral_false)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"false";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_with_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_FALSE, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(5, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_string)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\"234\"\n"
"\n";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(5, tok->loc.end);
    tok_clean(tok);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_string_with_escape)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\"\n\"\n"
"\n";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_with_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(3, tok->loc.end);
    ASSERT_STREQ("\n", tok->str_val);
    tok_clean(tok);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_string_with_raw_escape)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\"\\n\"\n"
"\n";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_with_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);
    ASSERT_STREQ("\n", tok->str_val);
    tok_clean(tok);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_utc_string)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\"你好\"\n"
                       "\n";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_STRING, tok->token_type);
    ASSERT_EQ(1, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(0, tok->loc.start);
    ASSERT_EQ(8, tok->loc.end);
    ASSERT_STREQ("你好", tok->str_val);
    tok_clean(tok);
    ASSERT_EQ(TOKEN_NEWLINE, get_tok(lexer)->token_type);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_num_int)
{
    struct frontend *fe = frontend_init();
    char test_code[] = 
"\n"
"234\n"
"\n";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

 TEST(test_lexer, token_num_int_hex)
 {
     struct frontend *fe = frontend_init();
     char test_code[] = "\n"
                        "0x2\n"
                        "\n";

     struct token *tok;
     struct lexer *lexer;
     lexer = lexer_new_for_string(test_code);
     tok = get_tok(lexer);
     ASSERT_EQ(TOKEN_INT, tok->token_type);
     ASSERT_EQ(2, tok->int_val);
     ASSERT_EQ(2, tok->loc.line);
     ASSERT_EQ(1, tok->loc.col);
     ASSERT_EQ(1, tok->loc.start);
     ASSERT_EQ(4, tok->loc.end);
     lexer_free(lexer);
     frontend_deinit(fe);
}

TEST(test_lexer, token_num_int_hex2)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n"
                       "0xffffffff\n"
                       "\n";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, tok->token_type);
    ASSERT_EQ(-1, tok->int_val);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_num_float)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n23.1";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_DOUBLE, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(5, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_num_float2)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n2.3";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_DOUBLE, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

TEST(test_lexer, token_num_float3)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n.23";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_DOUBLE, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

TEST(test_lexer, token_id)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n_abc123";
    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(8, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
 }

TEST(test_lexer, token_num_id)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n123 abc";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    //NUM
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(1, tok->loc.col);
    ASSERT_EQ(1, tok->loc.start);
    ASSERT_EQ(4, tok->loc.end);

    //IDENT
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_IDENT, tok->token_type);
    ASSERT_EQ(2, tok->loc.line);
    ASSERT_EQ(5, tok->loc.col);
    ASSERT_EQ(5, tok->loc.start);
    ASSERT_EQ(8, tok->loc.end);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, expr)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "10+20.0";

    struct token *tok;
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_INT, tok->token_type);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_OP, tok->token_type);
    ASSERT_EQ(OP_PLUS, tok->opcode);
    tok = get_tok(lexer);
    ASSERT_EQ(TOKEN_DOUBLE, tok->token_type);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, other_symbols)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "->( )()[]{} .. ... .< <= == != >= > || && ! |&/+-*^** *= /= %= += -= <<= >>= &= ^= |= ++ -- ? ///**/";

    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    //'('
    ASSERT_EQ(TOKEN_MAPTO, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_LPAREN, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_RPAREN, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_UNIT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_LBRACKET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_RBRACKET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_LCBRACKET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_RCBRACKET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_RANGE, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_VARIADIC, get_tok(lexer)->token_type);
    ASSERT_EQ(OP_DOT, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_LT, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_LE, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_EQ, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_NE, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_GE, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_GT, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_OR, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_AND, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_NOT, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_BITOR, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_BAND, get_tok(lexer)->opcode);

    ASSERT_EQ(OP_DIVISION, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_PLUS, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_MINUS, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_STAR, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_BITEXOR, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_POW, get_tok(lexer)->opcode);

    ASSERT_EQ(OP_MUL_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_DIV_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_MOD_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_ADD_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_SUB_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_LEFT_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_RIGHT_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_AND_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_XOR_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_OR_ASSN, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_INC, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_DEC, get_tok(lexer)->opcode);
    ASSERT_EQ(OP_COND, get_tok(lexer)->opcode);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_indent_dedent)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "\n  abc";

    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    ASSERT_EQ(TOKEN_INDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_DEDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_EOF, get_tok(lexer)->token_type);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, token_import_memory)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "from sys import memory";

    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    ASSERT_EQ(TOKEN_FROM, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IDENT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_IMPORT, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_MEMORY, get_tok(lexer)->token_type);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, complex_number)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "3.0 + 1.0i";

    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    ASSERT_EQ(TOKEN_COMPLEX, get_tok(lexer)->token_type);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, group_with_multiple_lines)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "[\n\
    ]";

    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);

    ASSERT_EQ(TOKEN_LBRACKET, get_tok(lexer)->token_type);
    ASSERT_EQ(TOKEN_RBRACKET, get_tok(lexer)->token_type);

    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, highlight_code_empty_string)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "";
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    const char *highlighted = highlight(lexer, test_code);
    ASSERT_STREQ("", highlighted);
    free((void*)highlighted);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, highlight_code)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x";
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    const char *highlighted = highlight(lexer, test_code);
    ASSERT_STREQ("x", highlighted);
    free((void*)highlighted);
    lexer_free(lexer);
    frontend_deinit(fe);
}

TEST(test_lexer, highlight_code_multi_lines)
{
    struct frontend *fe = frontend_init();
    char test_code[] = "x\n\tx\n\t\tx\n\t\t\n";
    struct lexer *lexer;
    lexer = lexer_new_for_string(test_code);
    const char *highlighted = highlight(lexer, test_code);
    ASSERT_STREQ(test_code, highlighted);
    free((void*)highlighted);
    lexer_free(lexer);
    frontend_deinit(fe);
}

int test_lexer()
{
    UNITY_BEGIN();
    RUN_TEST(test_lexer_empty_string);
    RUN_TEST(test_lexer_skip_comment);
    RUN_TEST(test_lexer_skip_comment_in_func);
    RUN_TEST(test_lexer_token_char);
    RUN_TEST(test_lexer_token_char_with_escape);
    RUN_TEST(test_lexer_token_bool_litteral_true);
    RUN_TEST(test_lexer_token_bool_litteral_false);
    RUN_TEST(test_lexer_token_string);
    RUN_TEST(test_lexer_token_string_with_escape);
    RUN_TEST(test_lexer_token_string_with_raw_escape);
    RUN_TEST(test_lexer_token_utc_string);
    RUN_TEST(test_lexer_token_id);
    RUN_TEST(test_lexer_token_num_float);
    RUN_TEST(test_lexer_token_num_float2);
    RUN_TEST(test_lexer_token_num_float3);
    RUN_TEST(test_lexer_token_num_id);
    RUN_TEST(test_lexer_token_num_int);
    RUN_TEST(test_lexer_token_num_int_hex);
    RUN_TEST(test_lexer_token_num_int_hex2);
    RUN_TEST(test_lexer_expr);
    RUN_TEST(test_lexer_other_symbols);
    RUN_TEST(test_lexer_token_indent_dedent);
    RUN_TEST(test_lexer_token_import_memory);
    RUN_TEST(test_lexer_complex_number);
    RUN_TEST(test_lexer_group_with_multiple_lines);
    RUN_TEST(test_lexer_highlight_code_empty_string);
    RUN_TEST(test_lexer_highlight_code);
    RUN_TEST(test_lexer_highlight_code_multi_lines);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
