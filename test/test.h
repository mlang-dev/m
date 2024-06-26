/*
 * test.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * test c header file
 */
#ifndef __CLIB_TEST_H__
#define __CLIB_TEST_H__


#include "unity.h"

#ifdef __cplusplus
extern "C" {
#endif

struct test_stats {
  int total_failures;
  int total_tests;
};

extern struct test_stats test_stats;

#define ASSERT_EQ(expected,  actual) TEST_ASSERT_EQUAL_INT(expected, actual) 
#define ASSERT_STREQ(expected,  actual) TEST_ASSERT_EQUAL_STRING(expected, actual) 
#define ASSERT_TRUE(condition) TEST_ASSERT_TRUE(condition)
#define ASSERT_FALSE(condition) TEST_ASSERT_FALSE(condition)

#define TEST(MODULE,  FUNC) void MODULE ## _ ## FUNC(void)

#ifdef __cplusplus
}
#endif

#endif
