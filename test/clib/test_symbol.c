/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "test.h"


#include "clib/generic.h"
#include "clib/hashtable.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "sema/type.h"

TEST(test_symbol, equals_to_same_string_key)
{
    symbols_init();
    char str1[] = "hello";
    char str2[] = "world";
    symbol symbol1 = to_symbol(str1);
    symbol symbol2 = to_symbol(str2);
    ASSERT_EQ(symbol1, to_symbol("hello"));
    ASSERT_EQ(symbol2, to_symbol("world"));
    symbols_deinit();
}

TEST(test_symbol, two_version)
{
    symbols_init();
    ASSERT_EQ(to_symbol2_0("world"), to_symbol("world"));
    symbols_deinit();
}

TEST(test_symbol, support_multiple_values_for_same_key)
{
    symbols_init();
    symbol symbol1 = to_symbol("hello");
    ASSERT_EQ(symbol1, to_symbol("hello"));
    symbols_deinit();
}

int test_symbol()
{
    UNITY_BEGIN();
    RUN_TEST(test_symbol_two_version);
    RUN_TEST(test_symbol_equals_to_same_string_key);
    RUN_TEST(test_symbol_support_multiple_values_for_same_key);
    return UNITY_END();
}
