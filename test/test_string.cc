/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "clib/string.h"
#include "gtest/gtest.h"

TEST(testString, testShortInit)
{
  string str;
  string_init_chars(&str, "hello world");
  ASSERT_STREQ("hello world", str.data);
  string_deinit(&str);
}

TEST(testString, testEmptyString)
{
  string str;
  string_init_chars(&str, "");
  ASSERT_STREQ("", str.data);
  ASSERT_TRUE(string_eq(&str, ""));
  string_deinit(&str);
}

TEST(testString, testEmptyString2)
{
  string str;
  string_init(&str);
  ASSERT_STREQ("", str.data);
  ASSERT_TRUE(string_eq(&str, ""));
  string_deinit(&str);
}

TEST(testString, testLongInit)
{
  string str;
  string_init_chars(&str, "hello world. this is very long string. expected to be dynamically allocated");
  ASSERT_STREQ("hello world. this is very long string. expected to be dynamically allocated", str.data);
  string_deinit(&str);
}

TEST(testString, testShortAppend)
{
  string str;
  string_init_chars(&str, "hello");
  string_add(&str, " world!");
  ASSERT_STREQ("hello world!", str.data);
  string_deinit(&str);
}

TEST(testString, testShortAppendToLong)
{
  string str;
  string_init_chars(&str, "hello");
  string_add(&str, " world!  this will become a very long string. ");
  ASSERT_STREQ("hello world!  this will become a very long string. ", str.data);
  string_deinit(&str);
}

TEST(testString, testLongAppendToLong)
{
  string str;
  string_init_chars(&str, "hello world!  this will become a very long string.");
  string_add(&str, " and more now");
  ASSERT_STREQ("hello world!  this will become a very long string. and more now", str.data);
  string_deinit(&str);
}

TEST(testString, testEq)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_TRUE(string_eq(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testNotEq)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_FALSE(string_eq(&str, "hello world."));
    string_deinit(&str);
}

TEST(testString, testNotEq2)
{
    string str;
    string_init_chars(&str, "hello world!");
    ASSERT_FALSE(string_eq(&str, "hello world!."));
    string_deinit(&str);
}

TEST(testString, testCopy)
{
    string str;
    char test_char[] = "hello world!";
    string_init(&str);
    string_copy_chars(&str, "hello world!");
    ASSERT_EQ(str.size, strlen(test_char));
    ASSERT_STREQ(str.data, "hello world!");
    ASSERT_TRUE(string_eq(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testSubstringUntil)
{
    string str;
    string_init_chars(&str, "abc.def.cfg");
    string_substr(&str, '.');
    ASSERT_STREQ(str.data, "abc.def");
    string_deinit(&str);
}

TEST(testString, testSubstringNoMatch)
{
    string str;
    string_init_chars(&str, "abcdef");
    string_substr(&str, '.');
    ASSERT_STREQ(str.data, "abcdef");
    string_deinit(&str);
}

TEST(testString, testSplitNoMatched)
{
    string str;
    string_init_chars(&str, "abcdefcfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(1, arr.size);
    ASSERT_STREQ("abcdefcfg", STRING_POINTER(array_get(&arr, 0))->data);
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testSplit)
{
    string str;
    string_init_chars(&str, "abc.def.cfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(3, arr.size);
    ASSERT_STREQ("abc", STRING_POINTER(array_get(&arr, 0))->data);
    ASSERT_STREQ("def", STRING_POINTER(array_get(&arr, 1))->data);
    ASSERT_STREQ("cfg", STRING_POINTER(array_get(&arr, 2))->data);
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testSplitLongString)
{
    string str;
    string_init_chars(&str, "this is very long string.def.cfg");
    array arr = string_split(&str, '.');
    ASSERT_EQ(3, arr.size);
    ASSERT_STREQ("this is very long string", STRING_POINTER(array_get(&arr, 0))->data);
    ASSERT_STREQ("def", STRING_POINTER(array_get(&arr, 1))->data);
    ASSERT_STREQ("cfg", STRING_POINTER(array_get(&arr, 2))->data);
    array_deinit(&arr);
    string_deinit(&str);
}

TEST(testString, testJoin)
{
    array arr;
    string str;
    string_array_init(&arr);
    string_init_chars(&str, "abc");
    array_insert(&arr, &str);
    string result = string_join(&arr, '.');
    ASSERT_STREQ(result.data, "abc");
    string_deinit(&str);
    array_deinit(&arr);
}

TEST(testString, testJoinMultiString)
{
    array arr;
    string str;
    string_array_init(&arr);
    string_init_chars(&str, "abc");
    array_insert(&arr, &str);
    string_copy_chars(&str, "def");
    array_insert(&arr, &str);
    string result = string_join(&arr, '.');
    ASSERT_STREQ(result.data, "abc.def");
    string_deinit(&str);
    array_deinit(&arr);
}

TEST(testString, testJoinMultiLongString)
{
    array arr;
    string str;
    string_array_init(&arr);
    string_init_chars(&str, "this is a very long string");
    array_insert(&arr, &str);
    string_copy_chars(&str, "this is a second very long string");
    array_insert(&arr, &str);
    string result = string_join(&arr, '.');
    ASSERT_STREQ(result.data, "this is a very long string.this is a second very long string");
    string_deinit(&str);
    array_deinit(&arr);
}
