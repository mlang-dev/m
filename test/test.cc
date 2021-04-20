#include "compiler/compiler.h"
#include "compiler/repl.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testCallByStruct)
{
    const char test_code[] = R"(
 type Point2D = x:double y:double
 f xy:Point2D = xy.x
 )";
    const char *expected_ir = R"(
%Point2D = type { double, double }

define double @f(%Point2D* %xy) {
entry:
  %x = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 0
  %xy.x = load double, double* %x, align 8
  ret double %xy.x
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
