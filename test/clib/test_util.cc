/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "clib/util.h"
#include "gtest/gtest.h"

TEST(testUtil, testIdNameGenerator)
{
    reset_id_name("a");
    ASSERT_STREQ("a", get_id_name()._reserved);
    ASSERT_STREQ("b", get_id_name()._reserved);
    reset_id_name("z");
    ASSERT_STREQ("z", get_id_name()._reserved);
    ASSERT_STREQ("aa", get_id_name()._reserved);
}

TEST(testUtil, testTwoLetters)
{
    reset_id_name("az");
    ASSERT_STREQ("az", get_id_name()._reserved);
    ASSERT_STREQ("ba", get_id_name()._reserved);
    ASSERT_STREQ("bb", get_id_name()._reserved);
}

TEST(testUtil, testMoreLetters)
{
    reset_id_name("zzzy");
    ASSERT_STREQ("zzzy", get_id_name()._reserved);
    ASSERT_STREQ("zzzz", get_id_name()._reserved);
    ASSERT_STREQ("aaaaa", get_id_name()._reserved);
}
#ifndef _WIN32
TEST(testUtil, testJoinPath)
{
    char path[64];
    join_path(path, "/usr/local/include", "stdio.h");
    ASSERT_STREQ("/usr/local/include/stdio.h", path);
}

TEST(testUtil, testJoinPath2)
{
    char path[64];
    join_path(path, "/usr/local/include/", "stdio.h");
    ASSERT_STREQ("/usr/local/include/stdio.h", path);
}
#endif
TEST(testUtil, testJoinPathEmpty)
{
    char path[64];
    join_path(path, "", "stdio.h");
    ASSERT_STREQ("stdio.h", path);
}

TEST(testUtil, testJoinPathNull)
{
    char path[64];
    join_path(path, 0, "stdio.h");
    ASSERT_STREQ("stdio.h", path);
}

TEST(testUtil, testJoinPathEmpty2)
{
    char path[64];
    join_path(path, "/usr/local/include/", "");
    ASSERT_STREQ("/usr/local/include/", path);
}

TEST(testUtil, testJoinPathNull2)
{
    char path[64];
    join_path(path, "/usr/local/include/", 0);
    ASSERT_STREQ("/usr/local/include/", path);
}
