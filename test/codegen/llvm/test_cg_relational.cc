/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for code generating
 */
#include "compiler/compiler.h"
#include "sema/analyzer.h"
#include "sema/sema_context.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>


TEST(testCGRelational, testLessThan)
{
    const char test_code[] = "def f(m:int n:int): m < n";
    const char *expected_ir = R"(
define i8 @f(i32 %m, i32 %n) {
entry:
  %n2 = alloca i32, align 4
  %m1 = alloca i32, align 4
  store i32 %m, i32* %m1, align 4
  store i32 %n, i32* %n2, align 4
  %m3 = load i32, i32* %m1, align 4
  %n4 = load i32, i32* %n2, align 4
  %cmplttmp = icmp slt i32 %m3, %n4
  %ret_val_int = zext i1 %cmplttmp to i8
  ret i8 %ret_val_int
}
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
