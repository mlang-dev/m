/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "clib/math.h"
#include "test.h"

TEST(test_math, pow)
{
    ASSERT_EQ(1, pow_int(2, 0));
    ASSERT_EQ(2, pow_int(2, 1));
    ASSERT_EQ(4, pow_int(2, 2));
    ASSERT_EQ(16, pow_int(2, 4));
    ASSERT_EQ(32, pow_int(2, 5));
    ASSERT_EQ(0, pow_int(2, -1));
}

int test_math(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_math_pow);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
