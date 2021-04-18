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
//  type Point2D = x:double y:double
//  f xy:Point2D = xy.x
//  )";
//     const char *expected_ir = R"(
//  %struct.Point2D = type { double, double }

//  define double @f(%struct.Point2D* %xy) #0 {
//  entry:
//    %x = getelementptr inbounds %struct.Point2D, %struct.Point2D* %xy, i32 0, i32 0
//    %0 = load double, double* %x, align 8
//    ret double %0
//  }
//  )";
//     validate_m_code_with_ir_code(test_code, expected_ir);
// }
