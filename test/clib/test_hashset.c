/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "test.h"

#include "clib/hashset.h"
#include "clib/object.h"
#include "clib/string.h"

TEST(test_hashset, add_and_check)
{
    hashset hs;
    hashset_init(&hs);
    const char* str1 = "hello hs1";
    const char* str2 = "hello hs2";
    const char* str3 = "no existing";
    hashset_set(&hs, str1);
    hashset_set(&hs, str2);
    ASSERT_EQ(2, hashset_size(&hs));
    ASSERT_TRUE(hashset_in(&hs, str1));
    ASSERT_TRUE(hashset_in(&hs, str2));
    ASSERT_FALSE(hashset_in(&hs, str3));
    hashset_deinit(&hs);
}

int test_hashset(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hashset_add_and_check);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
