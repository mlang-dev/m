#include "compiler/compiler.h"
#include "compiler/repl.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testReturnStructDirect)
{
    const char test_code[] = R"(
 type Point2D = x:double y:double
 f () = 
   xy:Point2D = 10.0 20.0
   xy
 )";
    const char *expected_ir = R"(
%Point2D = type { double, double }

define void @f(%Point2D* noalias sret(%Point2D) %agg.result) {
entry:
  %0 = getelementptr inbounds %Point2D, %Point2D* %agg.result, i32 0, i32 0
  store double 1.000000e+01, double* %0, align 8
  %1 = getelementptr inbounds %Point2D, %Point2D* %agg.result, i32 0, i32 1
  store double 2.000000e+01, double* %1, align 8
  ret void
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
