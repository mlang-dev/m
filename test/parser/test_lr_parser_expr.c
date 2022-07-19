/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for lr parser for alexpression
 */
#include "parser/grammar.h"
#include "parser/lr_parser.h"
#include "parser/m_grammar.h"
#include "parser/astdump.h"
#include "codegen/wat_codegen.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>

TEST(test_lr_parser_expr, arithmetic_exp)
{
    const char *m_grammar = 
        "s  = e             {}\n"
        "e  = e '+' t       {}\n"
        "   | t             {}\n"
        "t  = t '*' f       {}\n"
        "   | f             {}\n"
        "f  = '(' e ')'     {}\n"
        "   | IDENT         {}\n";

    const char test_code[] = "1+2";
    frontend_init();
    struct lr_parser *parser = lr_parser_new(m_grammar);
    struct ast_node *ast = parse_text(parser, test_code);
 //   string dump_str = dump(ast);
 //   ASSERT_STREQ(test_code, string_get(&dump_str));

    ast_node_free(ast);
    lr_parser_free(parser);
    frontend_deinit();
}

/*
TEST(test_lr_parser_expr, arithmetic_exp)
{
    const char *m_grammar = 
        "start  = e          { 0 }"
        "e      = e '+' t    { binop 0 1 2 }"
        "       | t              { 0 }"
        "t      = t '*' f { binop 0 1 2 }"
        "       | f            { 0 }"
        "f      = '(' e ')'      { 1 }"
        "       | INT       { 0 }";

    const char test_code[] = "1+2";
    const char expected[] = 
"\n"
"(i32.add\n"
"(i32.const 1)\n"
"(i32.const 2)\n"
")\n";
    frontend_init();
    wat_codegen_init();
    struct lr_parser *parser = lr_parser_new(m_grammar);
    struct ast_node *ast = parse_text(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    ast_node_free(ast);
    lr_parser_free(parser);
    frontend_deinit();
}

TEST(test_g_parser, arithmetic_exp1)
{
    const char test_code[] = "(1+2)";
    const char expected[] = 
"\n"
"(i32.add\n"
"(i32.const 1)\n"
"(i32.const 2)\n"
")\n";

    frontend_init();
    wat_codegen_init();
    struct earley_parser *parser = earley_parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    ast_node_free(ast);
    earley_parser_free(parser);
    frontend_deinit();
}

TEST(test_g_parser, arithmetic_exp2)
{
    const char test_code[] = "1 + 2 * 4";
    const char expected[] = 
"\n"
"(i32.add\n"
"(i32.const 1)\n"
"(i32.mul\n"
"(i32.const 2)\n"
"(i32.const 4)\n"
")\n"
")\n";
    frontend_init();
    wat_codegen_init();
    struct earley_parser *parser = earley_parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    ast_node_free(ast);
    earley_parser_free(parser);
    frontend_deinit();
}


TEST(test_g_parser, arithmetic_exp3)
{
    const char test_code[] = "1 * 2 + 3";
    const char expected[] = 
"\n"
"(i32.add\n"
"(i32.mul\n"
"(i32.const 1)\n"
"(i32.const 2)\n"
")\n"
"(i32.const 3)\n"
")\n";
    frontend_init();
    wat_codegen_init();
    struct earley_parser *parser = earley_parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    ast_node_free(ast);
    earley_parser_free(parser);
    frontend_deinit();
}

TEST(test_g_parser, arithmetic_exp4)
{
    const char test_code[] = "(1 + 2) * 3";
    const char expected[] = 
"\n"
"(i32.mul\n"
"(i32.add\n"
"(i32.const 1)\n"
"(i32.const 2)\n"
")\n"
"(i32.const 3)\n"
")\n";

    frontend_init();
    wat_codegen_init();
    struct earley_parser *parser = earley_parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    string_deinit(&code);
    ast_node_free(ast);
    earley_parser_free(parser);
    frontend_deinit();
}
*/

int test_lr_parser_expr()
{
    UNITY_BEGIN();
    RUN_TEST(test_lr_parser_expr_arithmetic_exp);
    //RUN_TEST(test_lr_parser_expr_arithmetic_exp1);
    //RUN_TEST(test_lr_parser_expr_arithmetic_exp2);
    //RUN_TEST(test_lr_parser_expr_arithmetic_exp3);
    //RUN_TEST(test_lr_parser_expr_arithmetic_exp4);
    return UNITY_END();
}
