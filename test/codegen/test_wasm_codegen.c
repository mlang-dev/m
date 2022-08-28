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
    struct wasm_module module;
    wasm_codegen_init(&module);
    char test_code[] = "\n\
x = 64\n\
x";
    parse_as_module(&module, test_code);
    ASSERT_TRUE(module.ba.size!=0);
    wasm_codegen_deinit(&module);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_parse_as_module);
    return UNITY_END();
}
