/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "ast.h"
#include "clib/array.h"
#include "clib/string.h"
#include <string.h>
#include "parser.h"
#include "tutil.h"
#include "astdump.h"

TEST(testAstDump, testPrototypeNodeDump)
{
    char test_code[] = "extern printf(format:string ...):int";
    auto parser = parser_new(false);
    block_node* block = parse_string(parser, "test", test_code);
    auto node = *(exp_node**)array_front(&block->nodes);
    ASSERT_EQ(PROTOTYPE_NODE, node->node_type);
    auto proto = (prototype_node*)node;
    ASSERT_STREQ("printf", string_get(&proto->name));
    string dump_str = dump(node);
    ASSERT_STREQ(test_code, string_get(&dump_str));
    parser_free(parser);
}

