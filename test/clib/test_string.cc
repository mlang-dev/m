/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for string functions
 */

#include "clib/string.h"
#include "gtest/gtest.h"

TEST(testString, testShortInit)
{
    string str;
    string_init_chars(&str, "hello world");
    ASSERT_EQ(11, string_size(&str));
    ASSERT_STREQ("hello world", string_get(&str));
    string_deinit(&str);
}

TEST(testString, testEmptyString)
{
    string str;
    string_init_chars(&str, "");
    ASSERT_STREQ("", string_get(&str));
    ASSERT_TRUE(string_eq_chars(&str, ""));
    string_deinit(&str);
}

TEST(testString, testEmptyString2)
{
    string str;
    string_init(&str);
    ASSERT_STREQ("", string_get(&str));
    ASSERT_TRUE(string_eq_chars(&str, ""));
    string_deinit(&str);
}

TEST(testString, testLongInit)
{
    string str;
    string_init_chars(&str, "hello world. this is very long string. expected to be dynamically allocated");
    ASSERT_STREQ("hello world. this is very long string. expected to be dynamically allocated", string_get(&str));
    string_deinit(&str);
}

TEST(testString, testShortAppend)
{
    string str;
    string_init_chars(&str, "hello");
    string_add_chars(&str, " world!");
    ASSERT_STREQ("hello world!", string_get(&str));
    string_deinit(&str);
}

TEST(testString, testShortAppendToLong)
{
    string str;
    string_init_chars(&str, "hello");
    string_add_chars(&str, " world!  this will become a very long string. ");
    ASSERT_STREQ("hello world!  this will become a very long string. ", string_get(&str));
    string_deinit(&str);
}

TEST(testString, testLongAppendToLong)
{
    string str;
    string_init_chars(&str, "hello world!  this will become a very long string.");
    string_add_chars(&str, " and more now");
    ASSERT_STREQ("hello world!  this will become a very long string. and more now", string_get(&str));
    string_deinit(&str);
}

TEST(testString, testEq)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_TRUE(string_eq_chars(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testNotEq)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_FALSE(string_eq_chars(&str, "hello world."));
    string_deinit(&str);
}

TEST(testString, testNotEq2)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_FALSE(string_eq_chars(&str, "hello world!."));
    string_deinit(&str);
}

TEST(testString, testCopy)
{
    string str;
    char test_char[] = "hello world!";
    string_init(&str);
    string_copy_chars(&str, "hello world!");
    ASSERT_EQ(string_size(&str), strlen(test_char));
    ASSERT_STREQ(string_get(&str), "hello world!");
    ASSERT_TRUE(string_eq_chars(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testSubstringUntil)
{
    string str;
    string_init_chars(&str, "abc.def.cfg");
    string_substr(&str, '.');
    ASSERT_STREQ(string_get(&str), "abc.def");
    string_deinit(&str);
}

TEST(testString, testSubstringNoMatch)
{
    string str;
    string_init_chars(&str, "abcdef");
    string_substr(&str, '.');
    ASSERT_STREQ(string_get(&str), "abcdef");
    string_deinit(&str);
}

TEST(testString, testSplitNoMatched)
{
    string str;
    string_init_chars(&str, "abcdefcfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(1, array_size(&arr));
    ASSERT_STREQ("abcdefcfg", string_get(STRING_POINTER(array_get(&arr, 0))));
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testSplit)
{
    string str;
    string_init_chars(&str, "abc.def.cfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(3, array_size(&arr));
    ASSERT_STREQ("abc", string_get(STRING_POINTER(array_get(&arr, 0))));
    ASSERT_STREQ("def", string_get(STRING_POINTER(array_get(&arr, 1))));
    ASSERT_STREQ("cfg", string_get(STRING_POINTER(array_get(&arr, 2))));
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testSplitLongString)
{
    string str;
    string_init_chars(&str, "this is a very long string.def.cfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(3, array_size(&arr));
    ASSERT_STREQ("this is a very long string", string_get(STRING_POINTER(array_get(&arr, 0))));
    ASSERT_STREQ("def", string_get(STRING_POINTER(array_get(&arr, 1))));
    ASSERT_STREQ("cfg", string_get(STRING_POINTER(array_get(&arr, 2))));
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testJoin)
{
    string str;
    ARRAY_STRING(arr);
    string_init_chars(&str, "abc");
    array_push(&arr, &str);
    string result = string_join(&arr, ".");
    ASSERT_STREQ(string_get(&result), "abc");
    string_deinit(&str);
    array_deinit(&arr);
}

TEST(testString, testJoinMultiString)
{
    string str;
    ARRAY_STRING(arr);
    string_init_chars(&str, "abc");
    array_push(&arr, &str);
    string_copy_chars(&str, "def");
    array_push(&arr, &str);
    string result = string_join(&arr, ".");
    ASSERT_STREQ(string_get(&result), "abc.def");
    string_deinit(&str);
    array_deinit(&arr);
}

TEST(testString, testJoinMultiLongString)
{
    string str;
    ARRAY_STRING(arr);
    string_init_chars(&str, "this is a very long string");
    array_push(&arr, &str);

    string_init(&str);
    string_copy_chars(&str, "this is a second very long string");
    array_push(&arr, &str);

    string result = string_join(&arr, ".");
    ASSERT_STREQ(string_get(&result), "this is a very long string.this is a second very long string");
    array_deinit(&arr);
}

TEST(testString, testShortStringAddShortString)
{
    string str1;
    string str2;
    string_init_chars(&str1, "hello ");
    string_init_chars(&str2, "world !");
    string_add(&str1, &str2);
    ASSERT_STREQ(string_get(&str1), "hello world !");
    string_deinit(&str1);
    string_deinit(&str2);
}

TEST(testString, testShortStringAddLongString)
{
    string str1;
    string str2;
    string_init_chars(&str1, "hello ");
    string_init_chars(&str2, "world ! this is a very long string.");
    string_add(&str1, &str2);
    ASSERT_STREQ(string_get(&str1), "hello world ! this is a very long string.");
    string_deinit(&str1);
    string_deinit(&str2);
}

TEST(testString, testLongStringAddLongString)
{
    string str1;
    string str2;
    string_init_chars(&str1, "hello this is the first long string");
    string_init_chars(&str2, " this is the second long string.");
    string_add(&str1, &str2);
    ASSERT_STREQ(string_get(&str1), "hello this is the first long string this is the second long string.");
    string_deinit(&str1);
    string_deinit(&str2);
}

TEST(testString, testLongStringAddShortString)
{
    string str1;
    string str2;
    string_init_chars(&str1, "this is the first long string");
    string_init_chars(&str2, " hello world !");
    string_add(&str1, &str2);
    ASSERT_STREQ(string_get(&str1), "this is the first long string hello world !");
    string_deinit(&str1);
    string_deinit(&str2);
}

TEST(testString, testSubstring)
{
    string str1;
    string_init_chars(&str1, "abc.h");
    ASSERT_STREQ("abc", string_get(string_substr(&str1, '.')));
    string_deinit(&str1);
}

TEST(testString, testInitLongStringWithCopyToShortString)
{
    string str;
    string_init_chars(&str, "this is a very long string");
    ASSERT_STREQ("this is a very long string", string_get(&str));
    string_copy_chars(&str, "r");
    ASSERT_STREQ("r", string_get(&str));
    string_add_chars(&str, "esult");
    ASSERT_STREQ("result", string_get(&str));
    string_deinit(&str);
}
