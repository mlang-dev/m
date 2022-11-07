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

class testCGVar : public TestBase {
};

TEST_F(testCGVar, testGlobalVarInt)
{
    const char test_code[] = "m = 10";
    const char *expected_ir = R"(
@m = global i32 10
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGVar, testGlobalVarChar)
{
    const char test_code[] = "m = 'm'";
    const char *expected_ir = R"(
@m = global i8 109
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGVar, testGlobalVarBool)
{
    const char test_code[] = "m = true";
    const char *expected_ir = R"(
@m = global i1 true
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGVar, testGlobalVarDouble)
{
    const char test_code[] = "m = 10.0";
    const char *expected_ir = R"(
@m = global double 1.000000e+01
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGVar, testGlobalVarString)
{
    const char test_code[] = R"(m = "10.0")";
    const char *expected_ir = R"(
@0 = private unnamed_addr constant [5 x i8] c"10.0\00", align 1
@m = global i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0)
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST_F(testCGVar, testGlobalVarStruct)
{
    const char test_code[] = R"(
struct Point2D = x:f64, y:f64
point:Point2D
)";
    const char *expected_ir = R"(
%Point2D = type { double, double }

@point = global %Point2D zeroinitializer
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}


TEST_F(testCGVar, testGlobalVarStructInitializer)
{
    const char test_code[] = R"(
struct Point2D = x:int, y:int
point:Point2D = Point2D(10, 20)
)";
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

@point = global %Point2D { i32 10, i32 20 }
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
