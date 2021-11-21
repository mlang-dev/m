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
    const char test_code[] = "1+2";
    const char expected[] = R"(
(module
(func $run (result i32)
i32.const 1
i32.const 2
i32.add
)
(export "run" (func $run))
)
)";
    string code = parse_exp_as_module("1+2");
    ASSERT_STREQ(expected, to_c_str(&code));
}
