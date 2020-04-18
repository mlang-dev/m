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
  string_init(&str, "hello world");
  ASSERT_STREQ("hello world", str.data);
  string_deinit(&str);
}


TEST(testString, testLongInit)
{
  string str;
  string_init(&str, "hello world. this is very long string. expected to be dynamically allocated");
  ASSERT_STREQ("hello world. this is very long string. expected to be dynamically allocated", str.data);
  string_deinit(&str);
}

TEST(testString, testShortAppend)
{
  string str;
  string_init(&str, "hello");
  string_add(&str, " world!");
  ASSERT_STREQ("hello world!", str.data);
  string_deinit(&str);
}

TEST(testString, testShortAppendToLong)
{
  string str;
  string_init(&str, "hello");
  string_add(&str, " world!  this will become a very long string. ");
  ASSERT_STREQ("hello world!  this will become a very long string. ", str.data);
  string_deinit(&str);
}

TEST(testString, testLongAppendToLong)
{
  string str;
  string_init(&str, "hello world!  this will become a very long string.");
  string_add(&str, " and more now");
  ASSERT_STREQ("hello world!  this will become a very long string. and more now", str.data);
  string_deinit(&str);
}

TEST(testString, testEq)
{
    string str;
    string_init(&str, "hello world!");
    ASSERT_TRUE(string_eq(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testNotEq)
{
    string str;
    string_init(&str, "hello world!");
    ASSERT_FALSE(string_eq(&str, "hello world."));
    string_deinit(&str);
}

TEST(testString, testNotEq2)
{
    string str;
    string_init(&str, "hello world!");
    ASSERT_FALSE(string_eq(&str, "hello world!."));
    string_deinit(&str);
}

TEST(testString, testCopy)
{
    string str;
    char test_char[] = "hello world!";
    string_init(&str, "");
    string_copy(&str, "hello world!");
    ASSERT_EQ(str.size, strlen(test_char));
    ASSERT_STREQ(str.data, "hello world!");
    ASSERT_TRUE(string_eq(&str, "hello world!"));
    string_deinit(&str);
}

TEST(testString, testSubstringUntil)
{
    string str;
    string_init(&str, "abc.def.cfg");
    substr_until(&str, '.');
    ASSERT_STREQ(str.data, "abc.def");
    string_deinit(&str);
}

TEST(testString, testJoin)
{
    string str;
    string_init(&str, "abc.def.cfg");
    substr_until(&str, '.');
    ASSERT_STREQ(str.data, "abc.def");
    string_deinit(&str);
}

TEST(testString, testSplit)
{
    string str;
    string_init(&str, "abc.def.cfg");
    substr_until(&str, '.');
    ASSERT_STREQ(str.data, "abc.def");
    string_deinit(&str);
}
