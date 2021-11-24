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
#include "test_base.h"
#include "tutil.h"

class testAstDump : public TestBase {
};

TEST_F(testAstDump, testPrototypeNodeDump)
{
    char test_code[] = "extern printf(format:string ...):int";
    auto env = env_new(false);
    block_node *block = parse_string(env->parser, "test", test_code);
    auto node = *(exp_node **)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    auto proto = (prototype_node *)node;
    ASSERT_STREQ("printf", string_get((string *)proto->name));
    string dump_str = dump(node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    env_free(env);
}
