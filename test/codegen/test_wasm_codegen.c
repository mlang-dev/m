/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
//#include "wasm/mwat.h"
#include "clib/util.h"
#include "clib/typedef.h"
#include "test.h"
#include "codegen/wasm_codegen.h"
#include <stdio.h>


TEST(test_wasm_codegen, parse_as_module)
{
    struct byte_array ba = parse_as_module("10+20");
    ASSERT_TRUE(ba.size!=0);
    ba_deinit(&ba);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_parse_as_module);
    return UNITY_END();
}
