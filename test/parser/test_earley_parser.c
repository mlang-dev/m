/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/earley_parser.h"
#include "parser/m_grammar.h"
#include "codegen/wat_codegen.h"
#include "test.h"
#include <stdio.h>

TEST(test_g_parser, arithmetic_exp)
{
    const char test_code[] = "1+2";
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

int test_g_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_g_parser_arithmetic_exp);
    RUN_TEST(test_g_parser_arithmetic_exp1);
    RUN_TEST(test_g_parser_arithmetic_exp2);
    RUN_TEST(test_g_parser_arithmetic_exp3);
    RUN_TEST(test_g_parser_arithmetic_exp4);
    return UNITY_END();
}
