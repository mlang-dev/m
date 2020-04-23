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
    hashtable_init(&ht, sizeof(object), sizeof(object));
    object obj1 = make_int(10);
    object val1 = make_int(100);
    object obj2 = make_int(20);
    object val2 = make_int(200);
    object obj3 = make_int(30);
    hashtable_add(&ht, &obj1, &val1);
    hashtable_add(&ht, &obj2, &val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, &obj1));
    ASSERT_TRUE(hashtable_in(&ht, &obj2));
    ASSERT_EQ(100, ((object*)hashtable_get(&ht, &obj1))->i_data);
    ASSERT_EQ(200, ((object*)hashtable_get(&ht, &obj2))->i_data);
    ASSERT_FALSE(hashtable_in(&ht, &obj3));
    ASSERT_EQ(NULL, hashtable_get(&ht, &obj3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestStringKey)
{
    hashtable ht;
    hashtable_init(&ht, sizeof(string), sizeof(object));
    string obj1 = make_string("hello");
    object val1 = make_int(100);
    string obj2 = make_string("world");
    object val2 = make_int(200);
    string obj3 = make_string("something else");
    hashtable_add(&ht, &obj1, &val1);
    hashtable_add(&ht, &obj2, &val2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, &obj1));
    ASSERT_TRUE(hashtable_in(&ht, &obj2));
    ASSERT_EQ(100, ((object*)hashtable_get(&ht, &obj1))->i_data);
    ASSERT_EQ(200, ((object*)hashtable_get(&ht, &obj2))->i_data);
    ASSERT_FALSE(hashtable_in(&ht, &obj3));
    ASSERT_EQ(NULL, hashtable_get(&ht, &obj3));
    hashtable_deinit(&ht);
}
