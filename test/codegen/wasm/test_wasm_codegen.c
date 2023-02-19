/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
//#include "wasm/mwat.h"
#include "clib/util.h"
#include "clib/typedef.h"
#include "test.h"
#include "codegen/wasm/cg_wasm.h"
#include "compiler/engine.h"
#include <stdio.h>

u8 *_compile_code(const char *text)
{
    struct engine *engine = engine_wasm_new();
    struct cg_wasm *cg = (struct cg_wasm*)engine->be->cg;
    u8 *data = compile_to_wasm(engine, text);
    cg->ba.data = 0;
    engine_free(engine);
    return data;
}

TEST(test_wasm_codegen, ref_type)
{
    char test_code[] = "\n\
let i = 10\n\
let j = &i\n\
i\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, emit_generic_fun)
{
    char test_code[] = "\n\
let sq x = x * x //comments \n\
sq 10.0\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, emit_nested_for_loop)
{
    char test_code[] = "\n\
var sum = 0\n\
for i in 1..3\n\
    for j in 1..3\n\
        sum = sum + i * j\n\
sum";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, mutable_record_member)
{
    char test_code[] = "\n\
record Point2D = x:mut f64, y:f64\n\
let change z:Point2D = \n\
    z.x = z.x * 10.0\n\
    z\n\
var old_z = Point2D{10.0, 20.0}\n\
let new_z = change old_z\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, variant_member)
{
    char test_code[] = "\n\
variant A = x:int | y:int\n\
let a = A { 10 }\n\
a.y\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, pattern_match)
{
    char test_code[] = "\n\
let pm x = \n\
    match x with\n\
    | 0 -> 100\n\
    | 1 -> 200\n\
    | _ -> 300\n\
pm 0\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, print)
{
    char test_code[] = "\n\
print \"hello world\"\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, ternary_operator)
{
    char test_code[] = "\n\
let x = 10\n\
x ? 1 : 0\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, pattern_match_negative)
{
    char test_code[] = "\n\
let pm x =\n\
    match x with\n\
    | -1 -> 100\n\
    | 3 -> 200\n\
    | _ -> 300\n\
pm (-1)\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, pattern_match_variable)
{
    char test_code[] = "\n\
let pm x =\n\
    match x with\n\
    | -1 -> 100\n\
    | 3 -> 200\n\
    | y -> y + 300\n\
pm (-1)\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_ref_type);
    RUN_TEST(test_wasm_codegen_emit_generic_fun);
    RUN_TEST(test_wasm_codegen_emit_nested_for_loop);
    RUN_TEST(test_wasm_codegen_mutable_record_member);
    RUN_TEST(test_wasm_codegen_variant_member);
    RUN_TEST(test_wasm_codegen_pattern_match);
    RUN_TEST(test_wasm_codegen_print);
    RUN_TEST(test_wasm_codegen_ternary_operator);
    RUN_TEST(test_wasm_codegen_pattern_match_negative);
    RUN_TEST(test_wasm_codegen_pattern_match_variable);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
