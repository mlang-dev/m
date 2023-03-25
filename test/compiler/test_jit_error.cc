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
#include "error/error.h"
#include <stdio.h>

TEST(testJITError, test_var_def_error)
{
    char test_code[] = R"(
i = 0
)";
    struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm *)engine->be->cg;
    JIT *jit = build_jit(engine);
    struct ast_node *block = parse_code(engine->fe->parser, test_code);
    analyze(cg->base.sema_context, block);
    auto node = (struct ast_node *)array_front_ptr(&block->block->nodes);
    eval_statement(jit, node);
    struct error_report *er = get_last_error_report(engine->fe->sema_context);
    ASSERT_STREQ("variable i is not defined.", er->error_msg);
    node_free(block);
    jit_free(jit);
    engine_free(engine);
}
