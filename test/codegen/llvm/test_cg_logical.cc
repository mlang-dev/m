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
#include "test_fixture.h"
#include <stdio.h>


TEST_F(TestFixture, testCGLogicalNot)
{
    const char test_code[] = "def f(m:bool): not m";
    const char *expected_ir = R"(
define i8 @f(i8 %m) {
entry:
  %m1 = alloca i8, align 1
  store i8 %m, ptr %m1, align 1
  %m2 = load i8, ptr %m1, align 1
  %ret_val_bool = trunc i8 %m2 to i1
  %nottmp = xor i1 %ret_val_bool, true
  %ret_val_int = zext i1 %nottmp to i8
  ret i8 %ret_val_int
}
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}
