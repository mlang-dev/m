/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/array.h"
#include "clib/string.h"
#include "codegen/env.h"
#include "parser/ast.h"
#include "parser/astdump.h"
#include "sema/analyzer.h"
#include "test_base.h"
#include "tutil.h"

class testAstDump : public TestBase {
};

TEST_F(testAstDump, testPrototypeNodeDump)
{
    char test_code[] = "extern printf:int format:string ...";
    auto env = env_new(false);
    ast_node *block = parse_code(env->new_parser, test_code);
    analyze(env->cg->sema_context, block);
    auto node = *(ast_node **)array_front(&block->block->nodes);
    ASSERT_EQ(FUNC_TYPE_NODE, node->node_type);
    auto func_type = (ast_node *)node;
    ASSERT_STREQ("printf", string_get((string *)func_type->ft->name));
    string dump_str = dump(node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    env_free(env);
}
