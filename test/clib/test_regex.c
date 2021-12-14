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
    ASSERT_STREQ("_abcABC||||||_abcABC012|||||||||*.", to_postfix("[_a-cA-C][_a-cA-C0-2]*"));
}

TEST(test_regex, to_postfix_escaping_char)
{
    ASSERT_STREQ("\\[\\].", to_postfix("\\[\\]"));
}

TEST(test_regex, exact_match)
{
    void *re = regex_new("if|while");
    ASSERT_TRUE(regex_match(re, "if"));
    ASSERT_TRUE(regex_match(re, "while"));
    regex_free(re);
}

TEST(test_regex, ident_match)
{
    void *re = regex_new("if");
    ASSERT_TRUE(regex_match(re, "if"));
    regex_free(re);
}

int test_regex()
{
    UNITY_BEGIN();
    RUN_TEST(test_regex_to_postfix);
    RUN_TEST(test_regex_to_postfix_charset);
    RUN_TEST(test_regex_exact_match);
    RUN_TEST(test_regex_ident_match);
    RUN_TEST(test_regex_to_postfix_escaping_char);
    return UNITY_END();
}
