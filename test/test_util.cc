/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "util.h"
#include "gtest/gtest.h"

TEST(testUtil, testIdNameGenerator)
{
    reset_id_name();
    ASSERT_STREQ("a", get_id_name().data);
    ASSERT_STREQ("b", get_id_name().data);
    reset_id_name("z");
    ASSERT_STREQ("z", get_id_name().data);
    ASSERT_STREQ("aa", get_id_name().data);
}

TEST(testUtil, testTwoLetters)
{
    reset_id_name("az");
    ASSERT_STREQ("az", get_id_name().data);
    ASSERT_STREQ("ba", get_id_name().data);
    ASSERT_STREQ("bb", get_id_name().data);
}

TEST(testUtil, testMoreLetters)
{
    reset_id_name("zzzy");
    ASSERT_STREQ("zzzy", get_id_name().data);
    ASSERT_STREQ("zzzz", get_id_name().data);
    ASSERT_STREQ("aaaaa", get_id_name().data);
}
