/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/array.h"
#include "clib/string.h"
#include <string.h>
#include "ast.h"

TEST(testArray, testArrayInitInt)
{
  array arr;
  array_init(&arr, sizeof(int));
  int i=10, j=20;
  array_push(&arr, &i);
  array_push(&arr, &j);
  ASSERT_EQ(2, array_size(&arr));
  ASSERT_EQ(10, *((int*)array_get(&arr, 0)));
  ASSERT_EQ(20, *((int*)array_get(&arr, 1)));
  array_deinit(&arr);
}


TEST(testArray, testArrayOfString)
{
  array arr;
  array_string_init(&arr);
  string str1;
  string_init_chars(&str1, "hello");
  string str2;
  string_init_chars(&str2, "world");

  array_push(&arr, &str1);
  array_push(&arr, &str2);
  ASSERT_EQ(2, array_size(&arr));
  ASSERT_STREQ("hello", string_get(STRING_POINTER(array_get(&arr, 0))));
  ASSERT_STREQ("world", string_get(STRING_POINTER(array_get(&arr, 1))));

  string_deinit(&str1);
  string_deinit(&str2);
  array_deinit(&arr);
}

TEST(testArray, testArrayOfLongString)
{
  array arr;
  array_string_init(&arr);
  string str1;
  string_init_chars(&str1, "hello world. this is a long string allocated in heap");
  string str2;
  string_init_chars(&str2, "world");

  array_push(&arr, &str1);
  array_push(&arr, &str2);
  ASSERT_EQ(2, array_size(&arr));
  ASSERT_STREQ("hello world. this is a long string allocated in heap", string_get(STRING_POINTER(array_get(&arr, 0))));
  ASSERT_STREQ("world", string_get(STRING_POINTER(array_get(&arr, 1))));

  array_deinit(&arr);
}

TEST(testArray, testElementWithNoOverhead)
{
  array arr;
  array_init(&arr, sizeof(char*));
  char exp[] = "hello";
  array_push(&arr, &exp);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_STREQ("hello", (char*)array_get(&arr, 0));
}

TEST(testArray, testElementWithNoOverheadInt)
{
  array arr;
  array_init(&arr, sizeof(int));
  int i = 1000;
  array_push(&arr, &i);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_EQ(1000, *((int*)array_get(&arr, 0)));
}
