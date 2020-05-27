/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include <string.h>

#include "clib/generic.h"
#include "clib/hashtable.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/util.h"
#include "type.h"

// TEST(testHashtable, TestAddAndGet)
// {
//     hashtable ht;
//     hashtable_init(&ht, sizeof(int), sizeof(int));
//     int obj1 = 10;
//     int val1 = 100;
//     int obj2 = 20;
//     int val2 = 200;
//     int obj3 = 30;
//     hashtable_set(&ht, &obj1, &val1);
//     hashtable_add(&ht, &obj2, &val2);
//     ASSERT_EQ(2, hashtable_size(&ht));
//     ASSERT_TRUE(hashtable_in(&ht, &obj1));
//     ASSERT_TRUE(hashtable_in(&ht, &obj2));
//     ASSERT_EQ(100, *((int*)hashtable_get(&ht, &obj1)));
//     ASSERT_EQ(200, *((int*)hashtable_get(&ht, &obj2)));
//     ASSERT_FALSE(hashtable_in(&ht, &obj3));
//     ASSERT_EQ(0, hashtable_get(&ht, &obj3));
//     hashtable_deinit(&ht);
// }

TEST(testHashtable, TestStrGeneric)
{
    hashtable ht;
    hashtable_init(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    char str3[] = "something else";
    int i = 100, j = 200;
    hashtable_set(&ht, str1, &i);
    hashtable_set(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, str1));
    ASSERT_TRUE(hashtable_in(&ht, str2));
    ASSERT_EQ(100, *((int*)hashtable_get(&ht, str1)));
    ASSERT_EQ(200, *((int*)hashtable_get(&ht, str2)));
    ASSERT_FALSE(hashtable_in(&ht, str3));
    ASSERT_EQ(0, hashtable_get(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestRemove)
{
    hashtable ht;
    hashtable_init(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    char str3[] = "something else";
    int i = 100, j = 200;
    hashtable_set(&ht, str1, &i);
    hashtable_set(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, str1));
    ASSERT_TRUE(hashtable_in(&ht, str2));
    hashtable_remove(&ht, str1);
    ASSERT_FALSE(hashtable_in(&ht, str1));
    ASSERT_EQ(0, hashtable_get(&ht, str1));
    ASSERT_EQ(200, *((int*)hashtable_get(&ht, str2)));
    ASSERT_FALSE(hashtable_in(&ht, str3));
    ASSERT_EQ(0, hashtable_get(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestClear)
{
    hashtable ht;
    hashtable_init(&ht);
    char str1[] = "hello";
    char str2[] = "world";
    int i = 100, j = 200;
    hashtable_set(&ht, str1, &i);
    hashtable_set(&ht, str2, &j);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in(&ht, str1));
    ASSERT_TRUE(hashtable_in(&ht, str2));
    hashtable_clear(&ht);
    ASSERT_FALSE(hashtable_in(&ht, str1));
    ASSERT_FALSE(hashtable_in(&ht, str2));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestHashtableCollision)
{
    reset_id_name("a");
    hashtable ht;
    hashtable_init(&ht);
    int j = 99, k = 100;
    hashtable_set(&ht, "sin", &j);
    hashtable_set(&ht, "int", &k);
    ASSERT_EQ(99, *(int*)hashtable_get(&ht, "sin"));
    ASSERT_EQ(100, *(int*)hashtable_get(&ht, "int"));
    ASSERT_EQ(2, hashtable_size(&ht));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestHashtableGrowWithCollision)
{
    reset_id_name("a");
    hashtable ht;
    hashtable_init(&ht);
    char strs[20][20];
    int value[20];
    int j = 99, k = 100;
    hashtable_set(&ht, "sin", &j);
    hashtable_set(&ht, "int", &k);
    for (int i = 0; i < 18; i++) {
        value[i] = i;
        string str = get_id_name();
        strcpy(strs[i], string_get(&str));
        hashtable_set(&ht, strs[i], &value[i]);
    }
    ASSERT_EQ(0, *(int*)hashtable_get(&ht, strs[0]));
    ASSERT_EQ(10, *(int*)hashtable_get(&ht, strs[10]));
    ASSERT_EQ(99, *(int*)hashtable_get(&ht, "sin"));
    ASSERT_EQ(100, *(int*)hashtable_get(&ht, "int"));
    hashtable_deinit(&ht);
}

TEST(testHashtable, TestHashtablePointerKey)
{
    reset_id_name("a");
    hashtable ht;
    hashtable_init(&ht);
    type_oper* op1 = create_nullary_type(TYPE_INT);
    type_oper* op2 = create_nullary_type(TYPE_DOUBLE);
    type_oper* op3 = create_nullary_type(TYPE_BOOL);
    hashtable_set_p(&ht, op1, op1);
    hashtable_set_p(&ht, op2, op2);
    ASSERT_EQ(op1, hashtable_get_p(&ht, op1));
    ASSERT_EQ(op2, hashtable_get_p(&ht, op2));
    ASSERT_EQ(0, hashtable_get_p(&ht, op3));
    ASSERT_EQ(2, hashtable_size(&ht));
    hashtable_deinit(&ht);
    type_exp_free((type_exp*)op1);
    type_exp_free((type_exp*)op2);
    type_exp_free((type_exp*)op3);
}
