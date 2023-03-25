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

TEST(testJITArray, array_init_access)
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
    auto array_decl = (struct ast_node *)array_front_ptr(&block->block->nodes);
    //auto array_access = (struct ast_node *)array_back_ptr(&block->block->nodes);
    eval_statement(jit, array_decl);
    //eval_result result = eval_exp(jit, array_access);
    //ASSERT_EQ(10, result.i_value);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
