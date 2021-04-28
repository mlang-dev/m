/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include <string.h>

#include "clib/generic.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/symboltable.h"
#include "clib/util.h"
#include "parser/parser.h"
#include "sema/type.h"
#include "codegen/env.h"

TEST(testSymboltable, TestSymboltableSameKeyMultipleValues)
{
    struct env *env = env_new(false);
    symboltable st;
    symboltable_init(&st);
    symbol s = to_symbol("hello");
    type_oper *op1 = create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    symboltable_push(&st, s, op1);
    ASSERT_EQ(op1, symboltable_get(&st, s));
    type_oper *op2 = create_nullary_type(TYPE_DOUBLE, get_type_symbol(TYPE_DOUBLE));
    symboltable_push(&st, s, op2);
    ASSERT_EQ(op2, symboltable_get(&st, s));
    symbol s1 = symboltable_pop(&st);
    ASSERT_EQ(s, s1);
    ASSERT_EQ(op1, symboltable_get(&st, s));
    symboltable_pop(&st);
    ASSERT_EQ(NULL, symboltable_get(&st, s));

    //over pop, still fine
    s1 = symboltable_pop(&st);
    ASSERT_EQ(NULL, s1);
    ASSERT_EQ(NULL, symboltable_get(&st, s));
    symboltable_deinit(&st);
    type_exp_free((type_exp *)op1);
    type_exp_free((type_exp *)op2);
    env_free(env);
}

TEST(testSymboltable, TestSymboltableMultipleKeys)
{
    struct env *env = env_new(false);
    symboltable st;
    symboltable_init(&st);
    symbol s1 = to_symbol("hello");
    type_oper *op1 = create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    symboltable_push(&st, s1, op1);
    ASSERT_EQ(op1, symboltable_get(&st, s1));
    symbol s2 = to_symbol("world");
    type_oper *op2 = create_nullary_type(TYPE_DOUBLE, get_type_symbol(TYPE_DOUBLE));
    symboltable_push(&st, s2, op2);
    ASSERT_EQ(op1, symboltable_get(&st, s1));
    ASSERT_EQ(op2, symboltable_get(&st, s2));
    symbol s = symboltable_pop(&st);
    ASSERT_EQ(s2, s);
    ASSERT_EQ(false, has_symbol(&st, s2));
    s = symboltable_pop(&st);
    ASSERT_EQ(s1, s);
    ASSERT_EQ(false, has_symbol(&st, s1));
    //over pop, still fine
    s1 = symboltable_pop(&st);
    ASSERT_EQ(NULL, s1);
    ASSERT_EQ(NULL, symboltable_get(&st, s));
    symboltable_deinit(&st);
    type_exp_free((type_exp *)op1);
    type_exp_free((type_exp *)op2);
    env_free(env);
}
