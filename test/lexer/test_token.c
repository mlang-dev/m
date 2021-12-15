/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "lexer/token.h"
#include "test.h"
#include <stdio.h>


TEST(test_token, patterns)
{
    struct token_patterns tps = get_token_patterns();
    for(size_t i = 0; i < tps.pattern_count; i++){
        ASSERT_EQ(i, tps.patterns[i].token_opcode);
    }
}

int test_token()
{
    UNITY_BEGIN();
    RUN_TEST(test_token_patterns);
    return UNITY_END();
}
