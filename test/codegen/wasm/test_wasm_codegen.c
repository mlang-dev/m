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
    compile_to_wasm(engine, text);
    u8 *data = cg->ba.data;
    cg->ba.data = 0;
    engine_free(engine);
    return data;
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

TEST(test_wasm_codegen, mutable_struct_member)
{
    char test_code[] = "\n\
struct Point2D = var x:f64, y:f64\n\
let change z:Point2D = \n\
    z.x = z.x * 10.0\n\
    z\n\
old_z = Point2D(10.0, 20.0)\n\
new_z = change old_z\n\
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

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_emit_generic_fun);
    RUN_TEST(test_wasm_codegen_emit_nested_for_loop);
    RUN_TEST(test_wasm_codegen_mutable_struct_member);
    RUN_TEST(test_wasm_codegen_print);
    return UNITY_END();
}
