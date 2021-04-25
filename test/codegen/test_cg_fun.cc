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

// TEST(testCGFun, testStructInitializer)
// {
//     const char test_code[] = R"(
//  type Point2D = x:int y:int
//  f () =
//    xy:Point2D = 10 20
//    xy.x
//  )";
//     const char *expected_ir = R"(
// %Point2D = type { i32, i32 }

// @__const.f.xy = private unnamed_addr constant %Point2D { i32 10, i32 20 }, align 4

// define i64 @f() {
// entry:
//   %retval = alloca %Point2D, align 4
//   %0 = bitcast %Point2D* %retval to i8*
//   call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %0, i8* align 4 bitcast (%Point2D* @@__const.f.xy to i8*), i64 8, i1 false)
//   %1 = bitcast %Point2D* %retval to i64*
//   %2 = load i64, i64* %1, align 4
//   ret i64 %2
// }
// )";
//     validate_m_code_with_ir_code(test_code, expected_ir);
// }

TEST(testCGFun, testPassStructIndirect)
{
    const char test_code[] = R"(
type Point2D = x:double y:double
f xy:Point2D = xy.y
)";
    const char *expected_ir = R"(
%Point2D = type { double, double }

define double @f(%Point2D* %xy) {
entry:
  %y = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 1
  %xy.y = load double, double* %y, align 8
  ret double %xy.y
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGFun, testPassStructDirect)
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

TEST(testCGFun, testReturnStructDirect)
{
    const char test_code[] = R"(
 type Point2D = x:int y:int
 f () = 
   xy:Point2D = 10 20
   xy
 )";
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

define i64 @f() {
entry:
  %xy = alloca %Point2D, align 4
  %0 = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 0
  store i32 10, i32* %0, align 4
  %1 = getelementptr inbounds %Point2D, %Point2D* %xy, i32 0, i32 1
  store i32 20, i32* %1, align 4
  %2 = bitcast %Point2D* %xy to i64*
  %3 = load i64, i64* %2, align 4
  ret i64 %3
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGFun, testReturnStructInDirect)
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
