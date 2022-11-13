/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for type inference and semantic analsysis
 */
#include "codegen/llvm/cg_llvm.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "error/error.h"
#include "compiler/engine.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testAnalyzerError, testNoFunctionFound)
{
    char test_code[] = R"(
no_exist_function_call ()
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);

    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(CALL_NODE, node->node_type);
    emit_code(cg, block);
    struct error_report *er = get_last_error_report(cg->base.sema_context);
    ASSERT_STREQ("function is not defined.", er->error_msg);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzerError, testRemError)
{
    char test_code[] = R"(
10+0.3
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(BINARY_NODE, node->node_type);
    emit_code(cg, (ast_node *)block);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzerError, tesTypeMismatch)
{
    char test_code[] = R"(
x:int = "string"
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(VAR_NODE, node->node_type);
    emit_code(cg, (ast_node *)block);
    struct error_report *er = get_last_error_report(cg->base.sema_context);
    ASSERT_STREQ("variable type not matched with literal constant.", er->error_msg);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzerError, testNonStructTypeFieldAccess)
{
    char test_code[] = R"(
x = 3
x.y
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    struct error_report* er = get_last_error_report(cg->base.sema_context);
    ASSERT_EQ(EC_EXPECT_STRUCT_TYPE, er->error_code);
    ASSERT_STREQ("The left side of the dot is expected to be a struct type.", er->error_msg);
    ast_node_free(block);
    engine_free(engine);
}

TEST(testAnalyzerError, testNonArrayTypeFieldAccess)
{
    char test_code[] = R"(
x = 3
x[3]
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    ASSERT_EQ(2, array_size(&block->block->nodes));
    analyze(cg->base.sema_context, block);
    struct error_report* er = get_last_error_report(cg->base.sema_context);
    ASSERT_EQ(EC_EXPECT_ARRAY_TYPE, er->error_code);
    ASSERT_STREQ("The left side of the bracket is expected to be an array type.", er->error_msg);
    ast_node_free(block);
    engine_free(engine);
}
