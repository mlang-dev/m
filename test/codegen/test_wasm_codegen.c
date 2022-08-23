/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for grammar parser
 */
#include "parser/grammar.h"
#include "parser/parser.h"
#include "codegen/wasm_codegen.h"
#include "lexer/init.h"
#include "test.h"
#include <stdio.h>
#include "wasm.h"

wasm_val_t _run(struct byte_array *ba)
{
    //wasm bytes initialization
    wasm_val_t vs[1];
    
    wasm_byte_vec_t wasm;
    wasm.size = ba->size;
    wasm.data = (wasm_byte_t *)ba->data;

    //creating store
    wasm_engine_t *engine = wasm_engine_new();
    wasm_store_t *store = wasm_store_new(engine);

    //compiling module 
    wasm_module_t *module = wasm_module_new(store, &wasm);
    if(!module) return vs[0];
    //creating imports
    wasm_extern_vec_t imports = WASM_EMPTY_VEC;
    wasm_trap_t *trap = 0;
    //Instantiating module
    wasm_instance_t *instance = wasm_instance_new(store, module, &imports, &trap);
    if(!instance)
        return vs[0];

    //Retrieving exports
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);
    if (exports.size == 0)
        return vs[0];
    wasm_func_t *func = wasm_extern_as_func(exports.data[0]);
    if (!func)
        return vs[0];
    //wasm_val_t args_val[2] = { WASM_I32_VAL(3), WASM_I32_VAL(4) };
    wasm_val_vec_t args = WASM_EMPTY_VEC;
    wasm_val_vec_t results = WASM_ARRAY_VEC(vs);
    if(wasm_func_call(func, &args, &results)) 
        return vs[0];
    
    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
    return vs[0];
}

TEST(test_wasm_codegen, func)
{
    const char test_code[] = "let run() = 10+20";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(30, _run(&wasm).of.i32);
    ba_deinit(&wasm);
}

TEST(test_wasm_codegen, func_exp)
{
    const char test_code[] = "let run() = (10+20) * 3";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(90, _run(&wasm).of.i32);
    ba_deinit(&wasm);
}

TEST(test_wasm_codegen, func_exp2)
{
    const char test_code[] = "let run() = 10+20 * 3";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(70, _run(&wasm).of.i32);
    ba_deinit(&wasm);
}

TEST(test_wasm_codegen, func_negative_num)
{
    const char test_code[] = "let run() = -10+20";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(10, _run(&wasm).of.i32);
    ba_deinit(&wasm);
}

TEST(test_wasm_codegen, add_float)
{
    const char test_code[] = "let run() = 10.0+20.0";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(30.0, _run(&wasm).of.f64);
    ba_deinit(&wasm);
}

TEST(test_wasm_codegen, add_float_negative)
{
    const char test_code[] = "let run() = -10.0+20.0";
    struct byte_array wasm = parse_as_module(test_code);
    ASSERT_EQ(10.0, _run(&wasm).of.f64);
    ba_deinit(&wasm);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_func);
    RUN_TEST(test_wasm_codegen_func_exp);
    RUN_TEST(test_wasm_codegen_func_exp2);
    RUN_TEST(test_wasm_codegen_func_negative_num);
    RUN_TEST(test_wasm_codegen_add_float);
    RUN_TEST(test_wasm_codegen_add_float_negative);
    return UNITY_END();
}
