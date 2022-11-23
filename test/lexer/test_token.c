/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/terminal.h"
#include "test.h"
#include <stdio.h>


TEST(test_token, patterns)
{
    struct token_patterns tps = get_token_patterns();
    for(size_t i = 0; i < TOKEN_OP + 1; i++){
        ASSERT_EQ(i, tps.patterns[i].token_type);
    }

    for(size_t i = 0; i < OP_TOTAL; i++){
        ASSERT_EQ(i, tps.patterns[TOKEN_OP + i].opcode);
    }
}

int test_token()
{
    UNITY_BEGIN();
    RUN_TEST(test_token_patterns);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
