/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/parser.h"
#include "parser/m_grammar.h"
#include "codegen/wasm_codegen.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGParser, testArithmeticExp)
{
    const char test_code[] = "1+2";
    const char expected[] = R"(
(i32.add
(i32.const 1)
(i32.const 2)
)
)";
    symbols_init();
    wasm_codegen_init();
    struct parser *parser = parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    symbols_deinit();
}


TEST(testGParser, testArithmeticExp1)
{
    const char test_code[] = "(1+2)";
    const char expected[] = R"(
(i32.add
(i32.const 1)
(i32.const 2)
)
)";
    symbols_init();
    wasm_codegen_init();
    struct parser *parser = parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    symbols_deinit();
}

TEST(testGParser, testArithmeticExp2)
{
    const char test_code[] = "1 + 2 * 4";
    const char expected[] = R"(
(i32.add
(i32.const 1)
(i32.mul
(i32.const 2)
(i32.const 4)
)
)
)";
    symbols_init();
    wasm_codegen_init();
    struct parser *parser = parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    symbols_deinit();
}


TEST(testGParser, testArithmeticExp3)
{
    const char test_code[] = "1 * 2 + 3";
    const char expected[] = R"(
(i32.add
(i32.mul
(i32.const 1)
(i32.const 2)
)
(i32.const 3)
)
)";
    symbols_init();
    wasm_codegen_init();
    struct parser *parser = parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    symbols_deinit();
}

TEST(testGParser, testArithmeticExp4)
{
    const char test_code[] = "(1 + 2) * 3";
    const char expected[] = R"(
(i32.mul
(i32.add
(i32.const 1)
(i32.const 2)
)
(i32.const 3)
)
)";
    symbols_init();
    wasm_codegen_init();
    struct parser *parser = parser_new(get_m_grammar());
    struct ast_node *ast = parse(parser, test_code);
    string code = generate(ast, test_code);
    ASSERT_STREQ(expected, to_c_str(&code));
    ast_node_free(ast);
    parser_free(parser);
    symbols_deinit();
}
