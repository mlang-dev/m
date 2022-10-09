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
let sum_sq z:cf64 = z.re ** 2.0 + z.im ** 2.0\n\
let ms z:cf64 c:cf64 = cf64(z.re ** 2.0 - z.im ** 2.0 + c.re, 2.0 * z.re * z.im + c.im)\n\
let converger z:cf64 iters:int c:cf64 = \n\
  if iters > 255 || (sum_sq z) > 4.0 then\n\
    iters\n\
  else\n\
    converger (ms z c) (iters + 1) c\n\
(ms (cf64(10.0, 20.0)) (cf64(10.0, 20.0))).im\n\
";
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
