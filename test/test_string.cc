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
