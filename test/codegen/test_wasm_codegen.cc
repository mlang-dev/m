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

TEST(testWasmCodegen, testFunc)
{
    const char test_code[] = "run()=1+2";
    const char expected[] = R"(
(func $run (result i32)
i32.const 1
i32.const 2
i32.add
)
(export "run" (func $run))
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
