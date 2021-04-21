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
 type Point2D = x:int y:int
 f xy:Point2D = xy.x
 )";
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

define i32 @f(i64 %xy.coerce) {
entry:
  %xy = alloca %Point2D, align 4
  %0 = bitcast %Point2D* %xy to i64*
  store i64 %xy.coerce, i64* %0, align 4
  %x = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 0
  %xy.x = load i32, i32* %x, align 4
  ret i32 %xy.x
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
