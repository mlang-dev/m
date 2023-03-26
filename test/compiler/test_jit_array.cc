/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for JIT for control statement and expressions
 */
#include "codegen/llvm/cg_llvm.h"
#include "compiler/engine.h"
#include "compiler/repl.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testJITArray, array_global_array_access)
{
    char test_code[] = R"(
var a = [10]
a[0]
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm *)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    eval_result result = eval_exp(jit, block);
    ASSERT_EQ(10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITArray, array_local_array_access)
{
    char test_code[] = R"(
let f() =
    var a = [10]
    a[0]
f()
)";
    struct engine *engine = engine_llvm_new(false);
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}

TEST(testJITArray, array_local_array_write_read)
{
    char test_code[] = R"(
let f() =
    var a = [10]
    a[0] = 20
    a[0]
f()
)";
    struct engine *engine = engine_llvm_new(false);
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_result result = eval_module(jit, block);
    ASSERT_EQ(20, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
