
/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib regex functions
 */
#include "test.h"
#include "clib/regex.h"

TEST(test_regex, to_postfix)
{
    ASSERT_STREQ("ab$", to_postfix("ab"));
    ASSERT_STREQ("ab$c$", to_postfix("abc"));
    ASSERT_STREQ("ab*$", to_postfix("ab*"));
    ASSERT_STREQ("ab$*", to_postfix("(ab)*"));
    ASSERT_STREQ("ab+$", to_postfix("ab+"));
    ASSERT_STREQ("ab?$", to_postfix("ab?"));
    ASSERT_STREQ("ab$c|", to_postfix("ab|c"));
    ASSERT_STREQ("abcd|||", to_postfix("a|b|c|d"));
    ASSERT_STREQ("ab$cd$|", to_postfix("ab|cd"));
    ASSERT_STREQ("\\+-|?", to_postfix("[\\+-]?"));
}

TEST(test_regex, to_postfix_charset)
{
    ASSERT_STREQ("ab|", to_postfix("[ab]"));
    ASSERT_STREQ("abc||", to_postfix("[abc]"));
    ASSERT_STREQ("a", to_postfix("[a-a]"));
    ASSERT_STREQ("ab|", to_postfix("[a-b]"));
    ASSERT_STREQ("abc||", to_postfix("[a-c]"));
    ASSERT_STREQ("_abcABC||||||", to_postfix("[_a-cA-C]"));
    ASSERT_STREQ("_abcABC||||||_abcABC012|||||||||*$", to_postfix("[_a-cA-C][_a-cA-C0-2]*"));
}

TEST(test_regex, to_postfix_escaping_char)
{
    ASSERT_STREQ("\\[\\]$", to_postfix("\\[\\]"));
    ASSERT_STREQ("\\[\\]|", to_postfix("\\[|\\]"));
    ASSERT_STREQ("0\\.$3$", to_postfix("0\\.3"));
}

TEST(test_regex, exact_match)
{
    void *re = regex_new("if|while");
    ASSERT_EQ(2, regex_match(re, "if", 0));
    ASSERT_EQ(5, regex_match(re, "while", 0));
    size_t matched;
    ASSERT_EQ(0, regex_match(re, "t", &matched));
    ASSERT_EQ(0, matched);
    ASSERT_EQ(0, regex_match(re, "i", &matched));
    ASSERT_EQ(1, matched);
    regex_free(re);
}

TEST(test_regex, exact_match_symbol)
{
    void *re = regex_new("!");
    ASSERT_EQ(1, regex_match(re, "! ", 0));
    regex_free(re);
}

TEST(test_regex, exact_match_escape)
{
    void *re = regex_new("\\.3");
    ASSERT_EQ(2, regex_match(re, ".3", 0));
    ASSERT_EQ(2, regex_match(re, ".30", 0));
    regex_free(re);
}

TEST(test_regex, exact_match_escape_braket)
{
    void *re = regex_new("\\[");
    ASSERT_EQ(1, regex_match(re, "[", 0));
    ASSERT_FALSE(regex_match(re, "]", 0));
    regex_free(re);
}

TEST(test_regex, ident_match)
{
    void *re = regex_new("[_a-zA-Z][_a-zA-Z0-9]*");
    ASSERT_EQ(6, regex_match(re, "_test3", 0));
    ASSERT_FALSE(regex_match(re, "3test", 0));
    regex_free(re);
}

TEST(test_regex, ident_match_space)
{
    void *re = regex_new("[_a-zA-Z][_a-zA-Z0-9]*");
    ASSERT_EQ(1, regex_match(re, "_", 0));
    ASSERT_EQ(6, regex_match(re, "_test3 !", 0));
    regex_free(re);
}

TEST(test_regex, float_num)
{
    void *re = regex_new("[\\+-]?([0-9]*.)?[0-9]+");
    ASSERT_EQ(3, regex_match(re, "0.3 !", 0));
    ASSERT_EQ(3, regex_match(re, "+.3 !", 0));
    ASSERT_EQ(3, regex_match(re, "-.3 !", 0));
    ASSERT_EQ(2, regex_match(re, "-3 !", 0));
    ASSERT_EQ(4, regex_match(re, "-0.3 !", 0));
    ASSERT_EQ(4, regex_match(re, "23.0", 0));
    ASSERT_EQ(4, regex_match(re, "-0.3. !", 0));
    ASSERT_EQ(7, regex_match(re, "0.12345 !", 0));

    regex_free(re);
}

TEST(test_regex, int_num)
{
    void *re = regex_new("[0-9]+|0x[0-9a-fA-F]+");
    ASSERT_EQ(1, regex_match(re, "3 !", 0));
    ASSERT_EQ(4, regex_match(re, "0x3F !", 0));
    ASSERT_EQ(5, regex_match(re, "12345 !", 0));

    regex_free(re);
}

int test_regex()
{
    UNITY_BEGIN();
    RUN_TEST(test_regex_to_postfix);
    RUN_TEST(test_regex_to_postfix_charset);
    RUN_TEST(test_regex_exact_match);
    RUN_TEST(test_regex_exact_match_symbol);
    RUN_TEST(test_regex_exact_match_escape);
    RUN_TEST(test_regex_ident_match);
    RUN_TEST(test_regex_ident_match_space);
    RUN_TEST(test_regex_to_postfix_escaping_char);
    RUN_TEST(test_regex_int_num);
    RUN_TEST(test_regex_float_num);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
