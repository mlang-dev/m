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

TEST(test_wasm_codegen, sample_code)
{
    char test_code[] = "\n\
def color_func iter_count iter_max sq_dist =\n\
    let mut v = 0.0, r = 0.0, g = 0.0, b = 0.0\n\
    if iter_count < iter_max then\n\
        v = (log(iter_count+1.5-(log2((log(sq_dist))/2.0))))/3.4\n\
        if v < 1.0 then \n\
            r = v ** 4;g = v ** 2.5;b = v\n\
        else\n\
            v = v < 2.0 ? 2.0 - v : 0.0\n\
            r = v;g = v ** 1.5;b = v ** 3.0\n\
    ((u8)(r * 255), (u8)(g * 255), (u8)(b * 255))\n\
\n\
def plot_mandelbrot_set x0:f64 y0:f64 x1:f64 y1:f64 =\n\
    let width = 400, height = 300\n\
    let mut a:u8[height][width * 4]\n\
    let scalex = (x1-x0)/width, scaley = (y1-y0)/height, max_iter = 510\n\
    for x in 0..width\n\
        for y in 0..height\n\
            let cx = x0 + scalex*x\n\
            let cy = y0 + scaley*y\n\
            let mut zx = 0.0, zy = 0.0\n\
            let mut zx2 = 0.0, zy2 = 0.0\n\
            let mut n = 0\n\
            while n<max_iter && (zx2 + zy2) < 4.0\n\
                zy = 2.0 * zx * zy + cy\n\
                zx = zx2  - zy2 + cx\n\
                zx2 = zx * zx\n\
                zy2 = zy * zy\n\
                n++\n\
            let r, g, b = color_func n max_iter (zx2 + zy2)\n\
            a[y][4*x] = r\n\
            a[y][4*x+1] = g\n\
            a[y][4*x+2] = b\n\
            a[y][4*x+3] = 255\n\
\n\
    setImageData a width height\n\
\n\
plot_mandelbrot_set (-2.0) (-1.2) 1.0 1.2\n\
    ";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
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
def sq x = x * x //comments \n\
sq 10.0\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, emit_nested_for_loop)
{
    char test_code[] = "\n\
let mut sum = 0\n\
for i in 1..3\n\
    for j in 1..3\n\
        sum = sum + i * j\n\
sum";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, mutable_struct_member)
{
    char test_code[] = "\n\
struct Point2D = x:mut f64, y:f64\n\
def change z:Point2D = \n\
    z.x = z.x * 10.0\n\
    z\n\
let mut old_z = Point2D{10.0, 20.0}\n\
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
def pm x = \n\
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
def pm x =\n\
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
def pm x =\n\
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

TEST(test_wasm_codegen, tuple)
{
    char test_code[] = "\n\
let t = (100, 200)\n\
t[0]\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, return_tuple)
{
    char test_code[] = "\n\
def t() = (100, 200)\n\
let x, y = t()\n\
x + y\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}


TEST(test_wasm_codegen, tuple_param)
{
    char test_code[] = "\n\
def a t:(int, int) = (100+t[0], 200+t[1])\n\
let x, y = a (100, 200)\n\
x + y\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

TEST(test_wasm_codegen, array_access)
{
    char test_code[] = "\n\
let mut a:u8[2] = [10, 20]\n\
a[0] = 30\n\
a[1] = 40\n\
a[0] + a[1]\n\
";
    u8 *wasm = _compile_code(test_code);
    ASSERT_TRUE(wasm);
    free(wasm);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_sample_code);
    RUN_TEST(test_wasm_codegen_ref_type);
    RUN_TEST(test_wasm_codegen_emit_generic_fun);
    RUN_TEST(test_wasm_codegen_emit_nested_for_loop);
    RUN_TEST(test_wasm_codegen_mutable_struct_member);
    RUN_TEST(test_wasm_codegen_variant_member);
    RUN_TEST(test_wasm_codegen_pattern_match);
    RUN_TEST(test_wasm_codegen_print);
    RUN_TEST(test_wasm_codegen_ternary_operator);
    RUN_TEST(test_wasm_codegen_pattern_match_negative);
    RUN_TEST(test_wasm_codegen_pattern_match_variable);
    RUN_TEST(test_wasm_codegen_tuple);
    RUN_TEST(test_wasm_codegen_return_tuple);
    RUN_TEST(test_wasm_codegen_tuple_param);
    RUN_TEST(test_wasm_codegen_array_access);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
