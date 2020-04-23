/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/array.h"
#include "clib/string.h"
#include <string.h>

TEST(testArray, testArrayInitInt)
{
  array arr;
  array_init(&arr, sizeof(object));
  object i=make_int(10), j=make_int(20);
  array_push(&arr, &i);
  array_push(&arr, &j);
  ASSERT_EQ(2, array_size(&arr));
  ASSERT_EQ(10, array_get(&arr, 0)->i_data);
  ASSERT_EQ(20, array_get(&arr, 1)->i_data);
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

  array_push(&arr, &str1.base);
  array_push(&arr, &str2.base);
  ASSERT_EQ(2, array_size(&arr));
  ASSERT_STREQ("hello", string_get(STRING_POINTER(array_get(&arr, 0))));
  ASSERT_STREQ("world", string_get(STRING_POINTER(array_get(&arr, 1))));

  string_deinit(&str1);
  string_deinit(&str2);
  array_deinit(&arr);
}

TEST(testArray, testRef)
{
  array arr;
  array_init(&arr, sizeof(object));
  char exp[] = "hello";
  object ref = make_ref(exp);
  array_push(&arr, &ref);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_STREQ("hello", (char*)array_get(&arr, 0)->p_data);
  array_deinit(&arr);
}


TEST(testArray, testElementWithNoOverhead)
{
  array arr;
  array_init(&arr, sizeof(char*));
  char exp[] = "hello";
  array_push_g(&arr, &exp);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_STREQ("hello", (char*)array_get_g(&arr, 0));
}

TEST(testArray, testElementWithNoOverheadInt)
{
  array arr;
  array_init(&arr, sizeof(int));
  int i = 1000;
  array_push_g(&arr, &i);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_EQ(1000, *((int*)array_get_g(&arr, 0)));
}
