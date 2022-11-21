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


TEST(test_wasm_codegen, emit_generic_fun)
{
    /*print \"%s %d\" \"hello world\" 10\n\
     */
    struct engine* engine = engine_wasm_new();
    struct cg_wasm *cg = engine->be->cg;
    char test_code[] = "\n\
let sq x = x * x //comments \n\
sq 10.0\n\
";
    compile_to_wasm(engine, test_code);
    ASSERT_TRUE(cg->ba.size!=0);
    engine_free(engine);
}

TEST(test_wasm_codegen, emit_nested_for_loop)
{
    /*print \"%s %d\" \"hello world\" 10\n\
     */
    struct engine* engine = engine_wasm_new();
    struct cg_wasm *cg = engine->be->cg;
    char test_code[] = "\n\
var sum = 0\n\
for i in 1..3\n\
    for j in 1..3\n\
        sum = sum + i * j\n\
sum";
    compile_to_wasm(engine, test_code);
    ASSERT_TRUE(cg->ba.size!=0);
    engine_free(engine);
}

TEST(test_wasm_codegen, mutable_struct_member)
{
    /*print \"%s %d\" \"hello world\" 10\n\
     */
    struct engine* engine = engine_wasm_new();
    struct cg_wasm *cg = engine->be->cg;
    char test_code[] = "\n\
struct Point2D = var x:f64, y:f64\n\
let change z:Point2D = \n\
    z.x = z.x * 10.0\n\
    z\n\
old_z = Point2D(10.0, 20.0)\n\
new_z = change old_z\n\
";
    compile_to_wasm(engine, test_code);
    ASSERT_TRUE(cg->ba.size!=0);
    engine_free(engine);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_emit_generic_fun);
    RUN_TEST(test_wasm_codegen_emit_nested_for_loop);
    RUN_TEST(test_wasm_codegen_mutable_struct_member);
    return UNITY_END();
}
