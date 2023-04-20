/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/ast.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "sema/frontend.h"

#include "test.h"
#include <stdio.h>


TEST(test_ast, node_type_names)
{
    struct node_type_name *ntns = get_node_type_names();
    for(size_t i = 0; i < TOTAL_NODE + 1; i++){
        ASSERT_EQ(i, ntns[i].node_type);
    }
}

TEST(test_ast_dump, prototype)
{
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(parser->tc, 0, false);
    char test_code[] = "fun printf format:string ... -> int";
    struct ast_node *block = parse_code(parser, test_code);
    analyze(context, block);
    struct ast_node *node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    struct ast_node *func_type = node;
    ASSERT_STREQ("printf", string_get((string *)func_type->ft->name));
    string dump_str = dump(context, node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    string_deinit(&dump_str);
    node_free(block);
    sema_context_free(context);
    parser_free(parser);
    frontend_deinit(fe);
}

TEST(test_ast_dump, func_type_no_parameter)
{
    struct frontend *fe = frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(parser->tc, 0, false);
    char test_code[] = "fun printf () -> int";
    struct ast_node *block = parse_code(parser, test_code);
    analyze(context, block);
    struct ast_node *node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    struct ast_node *func_type = node;
    ASSERT_STREQ("printf", string_get((string *)func_type->ft->name));
    string dump_str = dump(context, node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    string_deinit(&dump_str);
    node_free(block);
    sema_context_free(context);
    parser_free(parser);
    frontend_deinit(fe);
}

int test_ast()
{
    UNITY_BEGIN();
    RUN_TEST(test_ast_node_type_names);
    RUN_TEST(test_ast_dump_prototype);
    RUN_TEST(test_ast_dump_func_type_no_parameter);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
