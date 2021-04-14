#include "compiler/compiler.h"
#include "compiler/repl.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testBuildJit)
{
    env *env = env_new(true);
    JIT *jit = build_jit(env);
    jit_free(jit);
    env_free(env);
}

// TEST(testGeneral, testCallByStruct)
// {
//     const char test_code[] = R"(
// type Point2D = x:double y:double
// f x:Point2D = x.y
// )";
//     const char *expected_ir = R"(
// define i8 @__f_char(i8 %x) {
// entry:
//   %x1 = alloca i8, align 1
//   store i8 %x, i8* %x1, align 1
//   %x2 = load i8, i8* %x1, align 1
//   ret i8 %x2
// }

// %struct.Point2D = type { double, double }

// define double @f(%struct.Point2D* %xy) #0 {
// entry:
//   %x = getelementptr inbounds %struct.Point2D, %struct.Point2D* %xy, i32 0, i32 0
//   %0 = load double, double* %x, align 8
//   ret double %0
// }
// )";
//     validate_m_code_with_ir_code(test_code, expected_ir);
// }
