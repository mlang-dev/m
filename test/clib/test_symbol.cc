/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include <string.h>

#include "clib/generic.h"
#include "clib/hashtable.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/util.h"
#include "sema/type.h"
#include "clib/symbol.h"

TEST(testSymbol, TestSymbolEqualsWithTheSameStringKey)
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

TEST(testSymbol, TestSymbolSupportMultipleValuesForTheSameKey)
{
    symbols_init();
    symbol symbol1 = to_symbol("hello");
    ASSERT_EQ(symbol1, to_symbol("hello"));
    symbols_deinit();
}
