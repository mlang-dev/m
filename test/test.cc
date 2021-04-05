#include "sema/analyzer.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "compiler/repl.h"
#include "compiler/compiler.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testBuildJit)
{
    env* env = env_new(true);
    JIT* jit = build_jit(env);
    jit_free(jit);
    env_free(env);
}

// TEST(testGeneral, testCallByStruct)
// {
//     const char test_code[] = R"(
// type Point2D = x:double y:double
// f x:Point2D = x
// )";
//     const char *expected_ir = R"(
// define i8 @__f_char(i8 %x) {
// entry:
//   %x1 = alloca i8, align 1
//   store i8 %x, i8* %x1, align 1
//   %x2 = load i8, i8* %x1, align 1
//   ret i8 %x2
// }

// define i8 @main() {
// entry:
//   %calltmp = call i8 @__f_char(i8 99)
//   ret i8 %calltmp
// }
// )";
//     validate_m_code_with_ir_code(test_code, expected_ir);
// }
