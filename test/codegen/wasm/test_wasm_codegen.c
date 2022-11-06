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


TEST(test_wasm_codegen, parse_as_module)
{
    /*print \"%s %d\" \"hello world\" 10\n\
     */
    struct engine* engine = engine_wasm_new();
    struct cg_wasm *cg = engine->be->cg;
    char test_code[] = "\n\
a = [10]\n\
a[0] = 20\n\
a[0]\n\
";
//     char test_code[] = "\n\
// z = cf64(10.0, 20.0)\n\
// j = &z\n\
// j.im\n\
// ";

    compile_to_wasm(engine, test_code);
    ASSERT_TRUE(cg->ba.size!=0);
    engine_free(engine);
}

int test_wasm_codegen()
{
    UNITY_BEGIN();
    RUN_TEST(test_wasm_codegen_parse_as_module);
    return UNITY_END();
}
