/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib byte_array functions
 */
#include "test.h"
#include "clib/byte_array.h"

TEST(test_byte_array, init)
{
    struct byte_array ba;
    ba_init(&ba, 2);
    ba_add(&ba, 100);
    ba_add(&ba, 200);
    ASSERT_EQ(2, ba.size);
    ASSERT_EQ(100, ba.data[0]);
    ASSERT_EQ(200, ba.data[1]);
    ba_deinit(&ba);
}

int test_byte_array()
{
    UNITY_BEGIN();
    RUN_TEST(test_byte_array_init);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
