/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for calculating type size info
 */
#include "parser/parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>
#include "test_base.h"
#include "codegen/type_size_info.h"
#include "sema/env.h"
#include "sema/analyzer.h"

class testTypeSizeInfo : public TestBase {};

TEST_F(testTypeSizeInfo, testStructTypeSize)
{
    char test_code[] = "type Point2D = x:double y:double";
    env* env = env_new(false);
    block_node* block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    analyze(env, node);
    struct type_size_info tsi = get_type_size_info(env->cg, node->type);
    ASSERT_EQ(128, tsi.width);
    ASSERT_EQ(64, tsi.align);
    env_free(env);
}

