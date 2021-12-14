/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib regex functions
 */
#include "test.h"

#include "clib/regex.h"

TEST(test_regex, to_postfix)
{
    ASSERT_STREQ("ab.", to_postfix("ab"));
    ASSERT_STREQ("ab.c.", to_postfix("abc"));
    ASSERT_STREQ("ab*.", to_postfix("ab*"));
    ASSERT_STREQ("ab.*", to_postfix("(ab)*"));
    ASSERT_STREQ("ab+.", to_postfix("ab+"));
    ASSERT_STREQ("ab?.", to_postfix("ab?"));
    ASSERT_STREQ("ab.c|", to_postfix("ab|c"));
    ASSERT_STREQ("abcd|||", to_postfix("a|b|c|d"));
    ASSERT_STREQ("ab.cd.|", to_postfix("ab|cd"));
}

TEST(test_regex, to_postfix_charset)
{
    ASSERT_STREQ("ab|", to_postfix("[ab]"));
    ASSERT_STREQ("abc||", to_postfix("[abc]"));
    ASSERT_STREQ("a", to_postfix("[a-a]"));
    ASSERT_STREQ("ab|", to_postfix("[a-b]"));
    ASSERT_STREQ("abc||", to_postfix("[a-c]"));
    ASSERT_STREQ("_abcABC||||||", to_postfix("[_a-cA-C]"));
}

int test_regex()
{
    UNITY_BEGIN();
    RUN_TEST(test_regex_to_postfix);
    RUN_TEST(test_regex_to_postfix_charset);
    return UNITY_END();
}
