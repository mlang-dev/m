/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for code generating
 */
#include "compiler/compiler.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "test_base.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

class testCGCall : public TestBase {
};

TEST_F(testCGCall, testIntIdFunc)
{
    const char test_code[] = R"(
f x:int = x
)";
    const char *expected_ir = R"(
define i32 @f(i32 %x) {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  %x2 = load i32, i32* %x1, align 4
  ret i32 %x2
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGCall, testGenericIdFunc)
{
    const char test_code[] = R"(
f x = x
main () = f 'c'
)";
    const char *expected_ir = R"(
define i8 @__f_char(i8 %x) {
entry:
  %x1 = alloca i8, align 1
  store i8 %x, i8* %x1, align 1
  %x2 = load i8, i8* %x1, align 1
  ret i8 %x2
}

define i8 @main() {
entry:
  %0 = call i8 @__f_char(i8 99)
  ret i8 %0
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGCall, testTwoParamsFunc)
{
    const char test_code[] = R"(
f x y = (x + y) / 2
main () = f 10 20
)";
    const char *expected_ir = R"(
define i32 @f(i32 %x, i32 %y) {
entry:
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, i32* %x1, align 4
  store i32 %y, i32* %y2, align 4
  %x3 = load i32, i32* %x1, align 4
  %y4 = load i32, i32* %y2, align 4
  %addtmp = add i32 %x3, %y4
  %divtmp = sdiv i32 %addtmp, 2
  ret i32 %divtmp
}

define i32 @main() {
entry:
  %0 = call i32 @f(i32 10, i32 20)
  ret i32 %0
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGCall, testCallByStruct)
{
    const char test_code[] = R"(
type Point2D = x:double y:double
f x:Point2D = x.y
)";
    const char *expected_ir = R"(
define i8 @__f_char(i8 %x) {
entry:
  %x1 = alloca i8, align 1
  store i8 %x, i8* %x1, align 1
  %x2 = load i8, i8* %x1, align 1
  ret i8 %x2
}

%struct.Point2D = type { double, double }

define double @f(%struct.Point2D* %xy) #0 {
entry:
  %x = getelementptr inbounds %struct.Point2D, %struct.Point2D* %xy, i32 0, i32 0
  %0 = load double, double* %x, align 8
  ret double %0
}
)";
    //validate_m_code_with_ir_code(test_code, expected_ir);
}
