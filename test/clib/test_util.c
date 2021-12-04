/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for utility functions
 */
#include "clib/util.h"
#include "test.h"

TEST(test_util, id_name_generator)
{
    reset_id_name("a");
    ASSERT_STREQ("a", get_id_name()._reserved);
    ASSERT_STREQ("b", get_id_name()._reserved);
    reset_id_name("z");
    ASSERT_STREQ("z", get_id_name()._reserved);
    ASSERT_STREQ("aa", get_id_name()._reserved);
}

TEST(test_util, two_letters)
{
    reset_id_name("az");
    ASSERT_STREQ("az", get_id_name()._reserved);
    ASSERT_STREQ("ba", get_id_name()._reserved);
    ASSERT_STREQ("bb", get_id_name()._reserved);
}

TEST(test_util, more_letters)
{
    reset_id_name("zzzy");
    ASSERT_STREQ("zzzy", get_id_name()._reserved);
    ASSERT_STREQ("zzzz", get_id_name()._reserved);
    ASSERT_STREQ("aaaaa", get_id_name()._reserved);
}
#ifndef _WIN32
TEST(test_util, join_path)
{
    char path[64];
    join_path(path, sizeof(path), "/usr/local/include", "stdio.h");
    ASSERT_STREQ("/usr/local/include/stdio.h", path);
}

TEST(test_util, join_path2)
{
    char path[64];
    join_path(path, sizeof(path), "/usr/local/include/", "stdio.h");
    ASSERT_STREQ("/usr/local/include/stdio.h", path);
}
#endif
TEST(test_util, join_path_empty)
{
    char path[64];
    join_path(path, sizeof(path), "", "stdio.h");
    ASSERT_STREQ("stdio.h", path);
}

TEST(test_util, join_path_empty2)
{
    char path[64];
    join_path(path, sizeof(path), "/usr/local/include/", "");
    ASSERT_STREQ("/usr/local/include/", path);
}


TEST(test_util, join_path_null)
{
    char path[64];
    join_path(path, sizeof(path), 0, "stdio.h");
    ASSERT_STREQ("stdio.h", path);
}

TEST(test_util, join_path_null2)
{
    char path[64];
    join_path(path, sizeof(path), "/usr/local/include/", 0);
    ASSERT_STREQ("/usr/local/include/", path);
}

int test_util()
{
    UNITY_BEGIN();
    RUN_TEST(test_util_id_name_generator);
    RUN_TEST(test_util_two_letters);
    RUN_TEST(test_util_more_letters);
#ifndef _WIN32
    RUN_TEST(test_util_join_path);
    RUN_TEST(test_util_join_path2);
#endif
    RUN_TEST(test_util_join_path_empty);
    RUN_TEST(test_util_join_path_empty2);
    RUN_TEST(test_util_join_path_null);
    RUN_TEST(test_util_join_path_null2);
    return UNITY_END();
}
