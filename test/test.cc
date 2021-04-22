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

define i64 @f() {
entry:
  %xy = alloca %Point2D, align 8
  %0 = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 0
  store i32 10, i32* %0, align 4
  %1 = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 1
  store i32 20, i32* %1, align 4
  %2 = bitcast %Point2D* %xy to i64*
  %3 = load i64, i64* %2, align 4
  ret i64 %3
}
)";
    //validate_m_code_with_ir_code(test_code, expected_ir);
}
