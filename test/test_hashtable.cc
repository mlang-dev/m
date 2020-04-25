/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include <string.h>

#include "clib/object.h"
#include "clib/hashtable.h"
#include "clib/string.h"
#include "clib/generic.h"

TEST(testHashtable, TestAddAndGet)
{
    hashtable ht;
    hashtable_init(&ht, sizeof(int), sizeof(int));
    int obj1 = 10;
    int val1 = 100;
    int obj2 = 20;
    int val2 = 200;
    int obj3 = 30;
    hashtable_add(&ht, &obj1, &val1);
    hashtable_add(&ht, &obj2, &val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, &obj1));
    ASSERT_TRUE(hashtable_in(&ht, &obj2));
    ASSERT_EQ(100, *((int*)hashtable_get(&ht, &obj1)));
    ASSERT_EQ(200, *((int*)hashtable_get(&ht, &obj2)));
    ASSERT_FALSE(hashtable_in(&ht, &obj3));
    ASSERT_EQ(NULL, hashtable_get(&ht, &obj3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestStringKey)
{
    hashtable ht;
    hashtable_init_ref(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    char str3[] = "something else";
    value_ref obj1 = {str1, strlen(str1) + 1};
    int i = 100, j = 200;
    value_ref val1 = {&i, sizeof(int)};
    value_ref obj2 = {str2, strlen(str2) + 1};
    value_ref val2 = {&j, sizeof(int)};
    value_ref obj3 = {str3, strlen(str3) + 1};
    hashtable_add_ref(&ht, obj1, val1);
    hashtable_add_ref(&ht, obj2, val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_ref(&ht, obj1));
    ASSERT_TRUE(hashtable_in_ref(&ht, obj2));
    ASSERT_EQ(100, *((int*)hashtable_get_ref(&ht, obj1)));
    ASSERT_EQ(200, *((int*)hashtable_get_ref(&ht, obj2)));
    ASSERT_FALSE(hashtable_in_ref(&ht, obj3));
    ASSERT_EQ(NULL, hashtable_get_ref(&ht, obj3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestStrGeneric)
{
    hashtable ht;
    hashtable_init_ref(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    char str3[] = "something else";
    int i = 100, j = 200, k=300;
    hashtable_set_p(&ht, str1, &i);
    hashtable_set_p(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_p(&ht, str1));
    ASSERT_TRUE(hashtable_in_p(&ht, str2));
    ASSERT_EQ(100, *((int*)hashtable_get_p(&ht, str1)));
    ASSERT_EQ(200, *((int*)hashtable_get_p(&ht, str2)));
    ASSERT_FALSE(hashtable_in_p(&ht, str3));
    ASSERT_EQ(NULL, hashtable_get_p(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestRemove)
{
    hashtable ht;
    hashtable_init_ref(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    char str3[] = "something else";
    int i = 100, j = 200, k=300;
    hashtable_set_p(&ht, str1, &i);
    hashtable_set_p(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_p(&ht, str1));
    ASSERT_TRUE(hashtable_in_p(&ht, str2));
    hashtable_remove(&ht, str1);
    ASSERT_FALSE(hashtable_in_p(&ht, str1));
    ASSERT_EQ(NULL, hashtable_get_p(&ht, str1));
    ASSERT_EQ(200, *((int*)hashtable_get_p(&ht, str2)));
    ASSERT_FALSE(hashtable_in_p(&ht, str3));
    ASSERT_EQ(NULL, hashtable_get_p(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestClear)
{
    hashtable ht;
    hashtable_init_ref(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    int i = 100, j = 200;
    hashtable_set_p(&ht, str1, &i);
    hashtable_set_p(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_p(&ht, str1));
    ASSERT_TRUE(hashtable_in_p(&ht, str2));
    hashtable_clear(&ht);
    ASSERT_FALSE(hashtable_in_p(&ht, str1));
    ASSERT_FALSE(hashtable_in_p(&ht, str2));
    hashtable_deinit(&ht);
}