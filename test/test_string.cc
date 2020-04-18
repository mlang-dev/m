/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "clib/string.h"
#include "gtest/gtest.h"

TEST(testString, testConstructor)
{
  string str;
  string_init(&str, "hello world");
  ASSERT_STREQ("hello world", str.data);
  string_deinit(&str);
}
