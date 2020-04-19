/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/array.h"
#include "clib/string.h"


TEST(testArray, testArrayInitInt)
{
  array arr;
  array_init(&arr, sizeof(int));
  int i=10, j=20;
  array_push(&arr, &i);
  array_push(&arr, &j);
  ASSERT_EQ(2, arr.size);
  ASSERT_EQ(10, *((int*)array_get(&arr, 0)));
  ASSERT_EQ(20, *((int*)array_get(&arr, 1)));
  array_deinit(&arr);
}

TEST(testArray, testArrayOfString)
{
  array arr;
  array_init(&arr, sizeof(string));
  string str1;
  string_init_chars(&str1, "hello");
  string str2;
  string_init_chars(&str2, "world");

  array_push(&arr, &str1);
  array_push(&arr, &str2);
  ASSERT_EQ(2, arr.size);
  ASSERT_STREQ("hello", STRING_POINTER(array_get(&arr, 0))->data);
  ASSERT_STREQ("world", STRING_POINTER(array_get(&arr, 1))->data);

  string_deinit(&str1);
  string_deinit(&str2);
  array_deinit(&arr);
}
