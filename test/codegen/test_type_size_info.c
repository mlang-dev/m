/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for calculating type size info
 */
#include "codegen/type_size_info.h"
#include "codegen/backend.h"
#include "sema/analyzer.h"
#include "parser/parser.h"
#include "lexer/frontend.h"
#include "test.h"


TEST(test_type_size_info, struct_double_double)
{
    char test_code[] = "struct Point2D = x:double, y:double";
    frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    struct cg* cg = backend_init(context);
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    ast_node_free(block);
    parser_free(parser);
    sema_context_free(context);
    backend_deinit(cg);
    frontend_deinit();
}

TEST(test_type_size_info, struct_char_double)
{
    char test_code[] = "struct Point2D = x:char, y:double";
    frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    struct cg* cg = backend_init(context);
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    ast_node_free(block);
    parser_free(parser);
    sema_context_free(context);
    backend_deinit(cg);
    frontend_deinit();
}

TEST(test_type_size_info, struct_char_char)
{
    char test_code[] = "struct Point2D = x:char, y:char";
    frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    struct cg* cg = backend_init(context);
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    ast_node_free(block);
    parser_free(parser);
    sema_context_free(context);
    backend_deinit(cg);
    frontend_deinit();
}

TEST(test_type_size_info, struct_bool_char)
{
    char test_code[] = "struct Point2D = x:bool, y:char";
    frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    struct cg* cg = backend_init(context);
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    ast_node_free(block);
    parser_free(parser);
    sema_context_free(context);
    backend_deinit(cg);
    frontend_deinit();
}

TEST(test_type_size_info, struct_char_int)
{
    char test_code[] = "struct Point2D = x:char, y:int";
    frontend_init();
    struct parser *parser = parser_new();
    struct sema_context *context = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    struct cg* cg = backend_init(context);
    struct ast_node *block = parse_code(parser, test_code);
    struct ast_node *node = *(struct ast_node **)array_front(&block->block->nodes);
    analyze(cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(64, tsi.width_bits);
    ASSERT_EQ(32, tsi.align_bits);
    ast_node_free(block);
    parser_free(parser);
    sema_context_free(context);
    backend_deinit(cg);
    frontend_deinit();
}

int test_type_size_info()
{
    UNITY_BEGIN();
    RUN_TEST(test_type_size_info_struct_double_double);
    RUN_TEST(test_type_size_info_struct_char_double);
    RUN_TEST(test_type_size_info_struct_char_char);
    RUN_TEST(test_type_size_info_struct_bool_char);
    RUN_TEST(test_type_size_info_struct_char_int);
    return UNITY_END();
}
