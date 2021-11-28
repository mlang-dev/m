/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "wasm/mwat.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testMwat, testModFunc)
{
    const char expected[] = R"(
(module
(func $run (result i32)
(i32.add
(i32.const 10)
(i32.const 20)
)
)
(export "run" (func $run))
)
)";
    ASSERT_STREQ(expected, parse_exp_as_module("10+20"));
}
