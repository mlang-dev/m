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
#include "type.h"
#include "clib/symbol.h"

TEST(testSymbol, TestSymbolEqualsWithTheSameStringKey)
{
    hashtable symbols;
    hashtable_init(&symbols);
    char str1[] = "hello";
    char str2[] = "world";
    symbol symbol1 = to_symbol(&symbols, str1);
    symbol symbol2 = to_symbol(&symbols, str2);
    ASSERT_EQ(2, hashtable_size(&symbols));
    ASSERT_TRUE(hashtable_in(&symbols, str1));
    ASSERT_TRUE(hashtable_in(&symbols, str2));
    ASSERT_EQ(symbol1, to_symbol(&symbols, "hello"));
    ASSERT_EQ(symbol2, to_symbol(&symbols, "world"));
    hashtable_deinit(&symbols);
}

TEST(testSymbol, TestSymbolSupportMultipleValuesForTheSameKey)
{
    hashtable symbols;
    hashtable_init(&symbols);
    symbol symbol1 = to_symbol(&symbols, "hello");
    ASSERT_EQ(1, hashtable_size(&symbols));
    ASSERT_EQ(symbol1, to_symbol(&symbols, "hello"));
    hashtable_deinit(&symbols);
}
