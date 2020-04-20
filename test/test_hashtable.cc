/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include "clib/object.h"
#include "clib/hashtable.h"
#include "clib/string.h"

TEST(testHashtable, TestAddAndGet)
{
    hashtable ht;
    hashtable_init(&ht);
    object obj1 = box_int(10);
    object val1 = box_int(100);
    object obj2 = box_int(20);
    object val2 = box_int(200);
    object obj3 = box_int(30);
    hashtable_add(&ht, &obj1, &val1);
    hashtable_add(&ht, &obj2, &val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, &obj1));
    ASSERT_TRUE(hashtable_in(&ht, &obj2));
    ASSERT_EQ(100, hashtable_get(&ht, &obj1)->i_data);
    ASSERT_EQ(200, hashtable_get(&ht, &obj2)->i_data);
    ASSERT_FALSE(hashtable_in(&ht, &obj3));
    ASSERT_EQ(NULL, hashtable_get(&ht, &obj3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestStringKey)
{
    hashtable ht;
    hashtable_init(&ht);
    object obj1 = box_int(10);
    object val1 = box_int(100);
    object obj2 = box_int(20);
    object val2 = box_int(200);
    object obj3 = box_int(30);
    hashtable_add(&ht, &obj1, &val1);
    hashtable_add(&ht, &obj2, &val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, &obj1));
    ASSERT_TRUE(hashtable_in(&ht, &obj2));
    ASSERT_EQ(100, hashtable_get(&ht, &obj1)->i_data);
    ASSERT_EQ(200, hashtable_get(&ht, &obj2)->i_data);
    ASSERT_FALSE(hashtable_in(&ht, &obj3));
    ASSERT_EQ(NULL, hashtable_get(&ht, &obj3));
    hashtable_deinit(&ht);
}
