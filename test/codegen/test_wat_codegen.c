/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/parser.h"
#include "codegen/wat_codegen.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>

TEST(test_wat_codegen, func)
{
    const char test_code[] = "run()=1+2";
    const char expected[] = 
"\n"
"(func $run (result i32)\n"
"(i32.add\n"
"(i32.const 1)\n"
"(i32.const 2)\n"
")\n"
")\n"
"(export \"run\" (func $run))\n";
    frontend_init();
    wat_codegen_init();
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, test_code);
    string code = wat_generate(ast, test_code);
    ASSERT_STREQ(expected, string_get(&code));
    ast_node_free(ast);
    parser_free(parser);
    frontend_deinit();
}

int test_wat_codegen()
{
    UNITY_BEGIN();
    //RUN_TEST(test_wat_codegen_func);
    return UNITY_END();
}
