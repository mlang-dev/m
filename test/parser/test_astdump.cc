/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/array.h"
#include "clib/string.h"
#include "parser/ast.h"
#include "parser/astdump.h"
#include "parser/lalr_parser.h"
#include "sema/analyzer.h"
#include "lexer/init.h"
#include "test_base.h"
#include "tutil.h"

class testAstDump : public TestBase {
};

TEST_F(testAstDump, testPrototypeNodeDump)
{
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, false);
    char test_code[] = "extern printf:int format:string ...";
    ast_node *block = parse_code(parser, test_code);
    analyze(context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    auto func_type = (ast_node *)node;
    ASSERT_STREQ("printf", string_get((string *)func_type->ft->name));
    string dump_str = dump(node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    sema_context_free(context);
    lalr_parser_free(parser);
    frontend_deinit();
}

TEST_F(testAstDump, testFuncTypeWithNoParam)
{
    frontend_init();
    struct lalr_parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, false);
    char test_code[] = "extern printf:int ()";
    ast_node *block = parse_code(parser, test_code);
    analyze(context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    auto func_type = (ast_node *)node;
    ASSERT_STREQ("printf", string_get((string *)func_type->ft->name));
    string dump_str = dump(node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    sema_context_free(context);
    lalr_parser_free(parser);
    frontend_deinit();
}
