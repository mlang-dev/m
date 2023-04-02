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
#include "test_main.h"
#include "gtest/gtest.h"
#include "app/error.h"
#include <stdio.h>

TEST(testJITError, test_var_def_error)
{
    char test_code[] = R"(
i = 0
)";
    Environment *env = get_env();
    engine_reset(env->engine());
    struct ast_node *block = parse_code(env->engine()->fe->parser, test_code);
    block = split_ast_nodes_with_start_func(0, block);
    eval_module(env->jit(), block);
    struct error_report *er = get_last_error_report(env->engine()->fe->sema_context);
    ASSERT_STREQ("variable i is not defined.", er->error_msg);
    node_free(block);
}
