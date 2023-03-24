/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for calculating type size info
 */
#include "codegen/type_size_info.h"
#include "codegen/backend.h"
#include "sema/analyzer.h"
#include "parser/parser.h"
#include "sema/frontend.h"
#include "compiler/engine.h"
#include "test.h"


TEST(test_type_size_info, struct_double_double)
{
    char test_code[] = "struct Point2D = x:f64, y:f64";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_contains_struct)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
struct Contains = xy:Point2D\n\
";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(engine->fe->sema_context, block);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    node = array_back_ptr(&block->block->nodes);
    tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);

    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_refs_struct)
{
    char test_code[] = "\n\
struct Point2D = x:f64, y:f64\n\
struct Contains = xy:&Point2D\n\
";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(engine->fe->sema_context, block);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    node = array_back_ptr(&block->block->nodes);
    tsi = get_type_size_info(node->type);
    ASSERT_EQ(32, tsi.width_bits);
    ASSERT_EQ(32, tsi.align_bits);

    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_char_double)
{
    char test_code[] = "struct Point2D = x:char, y:f64";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_char_char)
{
    char test_code[] = "struct Point2D = x:char, y:char";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_bool_char)
{
    char test_code[] = "struct Point2D = x:bool, y:char";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, struct_char_int)
{
    char test_code[] = "struct Point2D = x:char, y:int";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(64, tsi.width_bits);
    ASSERT_EQ(32, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

TEST(test_type_size_info, variant_int)
{
    char test_code[] = "variant Point2D = x:char | y:int";
    struct engine *engine = engine_wasm_new();
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    struct ast_node *node = array_front_ptr(&block->block->nodes);
    analyze(engine->fe->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(32, tsi.width_bits);
    ASSERT_EQ(32, tsi.align_bits);
    node_free(block);
    engine_free(engine);
}

int test_type_size_info()
{
    UNITY_BEGIN();
    RUN_TEST(test_type_size_info_struct_double_double);
    RUN_TEST(test_type_size_info_struct_contains_struct);
    RUN_TEST(test_type_size_info_struct_refs_struct);
    RUN_TEST(test_type_size_info_struct_char_double);
    RUN_TEST(test_type_size_info_struct_char_char);
    RUN_TEST(test_type_size_info_struct_bool_char);
    RUN_TEST(test_type_size_info_struct_char_int);
    RUN_TEST(test_type_size_info_variant_int);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
