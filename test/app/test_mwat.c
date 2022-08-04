/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
//#include "wasm/mwat.h"
#include "clib/util.h"
#include "test.h"
#include <stdio.h>

TEST(test_mwat, parse_as_module)
{
//     const char expected[] = 
// "\n"
// "(module\n"
// "(func $run (result i32)\n"
// "(i32.add\n"
// "(i32.const 10)\n"
// "(i32.const 20)\n"
// ")\n"
// ")\n"
// "(export \"run\" (func $run))\n"
// ")\n";
    // const char *result = parse_exp_as_module("10+20");
    // ASSERT_STREQ(expected, result);
    // FREE((void*)result);
}

int test_mwat()
{
    UNITY_BEGIN();
    RUN_TEST(test_mwat_parse_as_module);
    return UNITY_END();
}
