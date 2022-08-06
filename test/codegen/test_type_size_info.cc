/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for calculating type size info
 */
#include "codegen/codegen.h"
#include "codegen/env.h"
#include "codegen/type_size_info.h"
#include "parser/m_parser.h"
#include "sema/analyzer.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

class testTypeSizeInfo : public TestBase {
};

TEST_F(testTypeSizeInfo, testStructTypeSize)
{
    char test_code[] = "type Point2D = x:double y:double";
    env *env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(env->cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    env_free(env);
}

TEST_F(testTypeSizeInfo, testStructTypeSizeCharDouble)
{
    char test_code[] = "type Point2D = x:char y:double";
    env *env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(env->cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(128, tsi.width_bits);
    ASSERT_EQ(64, tsi.align_bits);
    env_free(env);
}

TEST_F(testTypeSizeInfo, testStructTypeSizeCharChar)
{
    char test_code[] = "type Point2D = x:char y:char";
    env *env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(env->cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    env_free(env);
}

TEST_F(testTypeSizeInfo, testStructTypeSizeBoolChar)
{
    char test_code[] = "type Point2D = x:bool y:char";
    env *env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(env->cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(16, tsi.width_bits);
    ASSERT_EQ(8, tsi.align_bits);
    env_free(env);
}

TEST_F(testTypeSizeInfo, testStructTypeSizeCharInt)
{
    char test_code[] = "type Point2D = x:char y:int";
    env *env = env_new(false);
    ast_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    analyze(env->cg->sema_context, node);
    struct type_size_info tsi = get_type_size_info(node->type);
    ASSERT_EQ(64, tsi.width_bits);
    ASSERT_EQ(32, tsi.align_bits);
    env_free(env);
}
