/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "test.h"
#include "clib/array.h"
#include "clib/string.h"

TEST(test_array, init_int)
{
    struct array arr;
    array_init(&arr, sizeof(u32));
    u32 i = 10, j = 20;
    array_push_u32(&arr, i);
    array_push_u32(&arr, j);
    ASSERT_EQ(2, array_size(&arr));
    ASSERT_EQ(10, array_get_u32(&arr, 0));
    ASSERT_EQ(20, array_get_u32(&arr, 1));
    array_deinit(&arr);
}

int cmp(const void *elm1, const void *elm2)
{
    u32 f = *(u32*)elm1;
    u32 s = *(u32*)elm2;
    return f - s;
}

TEST(test_array, sort_int)
{
    struct array arr;
    array_init(&arr, sizeof(u32));
    u32 i = 20, j = 10;
    array_push_u32(&arr, i);
    array_push_u32(&arr, j);
    array_sort(&arr, cmp);
    ASSERT_EQ(2, array_size(&arr));
    ASSERT_EQ(10, array_get_u32(&arr, 0));
    ASSERT_EQ(20, array_get_u32(&arr, 1));
    array_deinit(&arr);
}

TEST(test_array, of_string)
{
    ARRAY_STRING(arr);
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

TEST(test_array, of_long_string)
{
    ARRAY_STRING(arr);
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

TEST(test_array, element_with_no_overhead)
{
    struct array arr;
    array_init(&arr, sizeof(char*));
    const char *exp = "hello";
    array_push(&arr, &exp);
    ASSERT_EQ(1, array_size(&arr));
    ASSERT_STREQ("hello", array_get_ptr(&arr, 0));
    array_deinit(&arr);
}

TEST(test_array, element_with_no_overhead_int)
{
    struct array arr;
    array_init(&arr, sizeof(int));
    int i = 1000;
    array_push(&arr, &i);
    ASSERT_EQ(1, array_size(&arr));
    ASSERT_EQ(1000, *((int*)array_get(&arr, 0)));
    array_deinit(&arr);
}

TEST(test_array, insert_at_begin)
{
    struct array arr;
    array_init(&arr, sizeof(char*));
    const char *exp = "hello";
    array_push(&arr, &exp);
    ASSERT_EQ(1, array_size(&arr));
    ASSERT_STREQ("hello", array_get_ptr(&arr, 0));
    const char *world = "world";
    array_insert_at(&arr, &world, 0);
    ASSERT_STREQ("world", array_get_ptr(&arr, 0));
    ASSERT_STREQ("hello", array_get_ptr(&arr, 1));
    array_deinit(&arr);
}

TEST(test_array, add_element_new_way)
{
    struct array arr;
    array_init(&arr, sizeof(char*));
    const char *exp = "hello";
    array_push_ptr(&arr, (void*)exp);
    ASSERT_EQ(1, array_size(&arr));
    ASSERT_STREQ("hello", array_get_ptr(&arr, 0));
    array_deinit(&arr);
}

int test_array()
{
    UNITY_BEGIN();
    RUN_TEST(test_array_init_int);
    RUN_TEST(test_array_sort_int);
    RUN_TEST(test_array_of_string);
    RUN_TEST(test_array_of_long_string);
    RUN_TEST(test_array_element_with_no_overhead);
    RUN_TEST(test_array_element_with_no_overhead_int);
    RUN_TEST(test_array_insert_at_begin);
    RUN_TEST(test_array_add_element_new_way);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
