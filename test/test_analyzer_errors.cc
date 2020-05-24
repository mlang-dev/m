/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "analyzer.h"
#include "codegen.h"
#include "parser.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(testAnalyzerError, testNoFunctionFound)
{
    char test_code[] = R"(
no_exist_function_call ()
)";
    testing::internal::CaptureStderr();
    menv* menv = create_env_for_string(test_code);
    block_node* block = parse_block(menv->parser, 0, 0, 0);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    type_env* env = menv->type_env;
    analyze(env, menv->cg, (exp_node*)block);
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_STREQ("error: :1:1: no_exist_function_call not defined\n", error.c_str());
    env_free(menv);
}
