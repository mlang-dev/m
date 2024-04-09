/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "test.h"


#include "clib/generic.h"
#include "clib/hashtable.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"

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

TEST(test_hashtable, str_int)
{
    struct hashtable ht;
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
    ASSERT_EQ(100, *((int *)hashtable_get(&ht, str1)));
    ASSERT_EQ(200, *((int *)hashtable_get(&ht, str2)));
    ASSERT_FALSE(hashtable_in(&ht, str3));
    ASSERT_EQ(0, hashtable_get(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(test_hashtable, symbol_int)
{
    struct hashtable ht;
    hashtable_init_with_value_size(&ht, sizeof(int), 0);
    symbol str1 = to_symbol("hello");
    symbol str2 = to_symbol("world");
    symbol str3 = to_symbol("something else");
    hashtable_set_int(&ht, str1, 100);
    hashtable_set_int(&ht, str2, 200);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_p(&ht, str1));
    ASSERT_TRUE(hashtable_in_p(&ht, str2));
    ASSERT_EQ(100, hashtable_get_int(&ht, str1));
    ASSERT_EQ(200, hashtable_get_int(&ht, str2));
    ASSERT_FALSE(hashtable_in_p(&ht, str3));
    ASSERT_EQ(-1, hashtable_get_int(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(test_hashtable, int_int)
{
    struct hashtable ht;
    hashtable_init_with_size(&ht, sizeof(int), sizeof(int));
    int key1 = 1000;
    int data1 = 100;
    int key2 = 2000;
    int data2 = 200;
    hashtable_set_v(&ht, &key1, &data1);
    hashtable_set_v(&ht, &key2, &data2);
    ASSERT_EQ(2, hashtable_size(&ht));
    ASSERT_TRUE(hashtable_in_v(&ht, &key1));
    ASSERT_TRUE(hashtable_in_v(&ht, &key2));
    ASSERT_EQ(100, *(int*)hashtable_get_v(&ht, &key1));
    ASSERT_EQ(200, *(int*)hashtable_get_v(&ht, &key2));
    hashtable_deinit(&ht);
}

TEST(test_hashtable, remove)
{
    struct hashtable ht;
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
    ASSERT_EQ(200, *((int *)hashtable_get(&ht, str2)));
    ASSERT_FALSE(hashtable_in(&ht, str3));
    ASSERT_EQ(0, hashtable_get(&ht, str3));
    hashtable_deinit(&ht);
}

TEST(test_hashtable, clear)
{
    struct hashtable ht;
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

TEST(test_hashtable, collision)
{
    reset_id_name("a");
    struct hashtable ht;
    hashtable_init(&ht);
    int j = 99, k = 100;
    hashtable_set(&ht, "sin", &j);
    hashtable_set(&ht, "int", &k);
    ASSERT_EQ(99, *(int *)hashtable_get(&ht, "sin"));
    ASSERT_EQ(100, *(int *)hashtable_get(&ht, "int"));
    ASSERT_EQ(2, hashtable_size(&ht));
    hashtable_deinit(&ht);
}

TEST(test_hashtable, grow_with_collision)
{
    reset_id_name("a");
    struct hashtable ht;
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
    ASSERT_EQ(0, *(int *)hashtable_get(&ht, strs[0]));
    ASSERT_EQ(10, *(int *)hashtable_get(&ht, strs[10]));
    ASSERT_EQ(99, *(int *)hashtable_get(&ht, "sin"));
    ASSERT_EQ(100, *(int *)hashtable_get(&ht, "int"));
    hashtable_deinit(&ht);
}
/*
TEST(testHashtable, TestHashtablePointerKey)
{
    struct struct engine *engine = engine_llvm_new(false);
    struct cg_llvm *cg = (struct cg_llvm*)engine->be->cg;
    reset_id_name("a");
    hashtable ht;
    hashtable_init(&ht);
    type_oper *op1 = create_nullary_type(TYPE_INT, get_type_symbol(TYPE_INT));
    type_oper *op2 = create_nullary_type(TYPE_F64, get_type_symbol(TYPE_F64));
    type_oper *op3 = create_nullary_type(TYPE_BOOL, get_type_symbol(TYPE_BOOL));
    hashtable_set_p(&ht, op1, op1);
    hashtable_set_p(&ht, op2, op2);
    ASSERT_EQ(op1, hashtable_get_p(&ht, op1));
    ASSERT_EQ(op2, hashtable_get_p(&ht, op2));
    ASSERT_EQ(0, hashtable_get_p(&ht, op3));
    ASSERT_EQ(2, hashtable_size(&ht));
    hashtable_deinit(&ht);
    type_item_free((type_item *)op1);
    type_item_free((type_item *)op2);
    type_item_free((type_item *)op3);
    engine_free(engine);
}

TEST(testHashtable, TestHashtablePointerKeyWithCopyValue)
{
    hashtable ht;
    hashtable_init_with_value_size(&ht, sizeof(struct type_size_info), 0);
    struct type_size_info tsi;
    tsi.width_bits = 64;
    tsi.align_bits = 64;
    symbol type_name = to_symbol("f64");
    hashtable_set_p(&ht, type_name, &tsi);
    struct type_size_info *result = (struct type_size_info *)hashtable_get_p(&ht, type_name);
    ASSERT_EQ(64, result->width_bits);
    ASSERT_EQ(64, result->align_bits);
    hashtable_deinit(&ht);
}
*/

int test_hashtable(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hashtable_str_int);
    RUN_TEST(test_hashtable_symbol_int);
    RUN_TEST(test_hashtable_int_int);
    RUN_TEST(test_hashtable_remove);
    RUN_TEST(test_hashtable_clear);
    RUN_TEST(test_hashtable_collision);
    RUN_TEST(test_hashtable_grow_with_collision);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
