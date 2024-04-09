/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "test.h"


#include "clib/generic.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/symboltable.h"
#include "clib/util.h"
#include "sema/type.h"

/*
TEST(test_symboltable, same_key_multiple_values)
{
    symboltable st;
    symboltable_init(&st);
    symbol s = to_symbol("hello");
    struct type_oper *op1 = create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    symboltable_push(&st, s, op1);
    ASSERT_EQ(op1, symboltable_get(&st, s));
    struct type_oper *op2 = create_nullary_type(TYPE_F64, get_type_symbol(TYPE_F64));
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
    type_item_free((struct type_item *)op1);
    type_item_free((struct type_item *)op2);
}

TEST(test_symboltable, multiple_keys)
{
    symboltable st;
    symboltable_init(&st);
    symbol s1 = to_symbol("hello");
    struct type_oper *op1 = create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    symboltable_push(&st, s1, op1);
    ASSERT_EQ(op1, symboltable_get(&st, s1));
    symbol s2 = to_symbol("world");
    struct type_oper *op2 = create_nullary_type(TYPE_F64, get_type_symbol(TYPE_F64));
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
    type_item_free((struct type_item *)op1);
    type_item_free((struct type_item *)op2);
}
*/

int test_symboltable(void)
{
    UNITY_BEGIN();
    //RUN_TEST(test_symboltable_multiple_keys);
    //RUN_TEST(test_symboltable_same_key_multiple_values);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
