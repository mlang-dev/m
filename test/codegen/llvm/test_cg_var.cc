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


TEST(testCGVar, testGlobalVarInt)
{
    const char test_code[] = "let m = 10";
    const char *expected_ir = R"(
@m = global i32 10
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarChar)
{
    const char test_code[] = "let m = 'm'";
    const char *expected_ir = R"(
@m = global i8 109
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarBool)
{
    const char test_code[] = "let m = true";
    const char *expected_ir = R"(
@m = global i1 true
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarDouble)
{
    const char test_code[] = "let m = 10.0";
    const char *expected_ir = R"(
@m = global double 1.000000e+01
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarString)
{
    const char test_code[] = R"(let m = "10.0")";
    const char *expected_ir = R"(
@0 = private unnamed_addr constant [5 x i8] c"10.0\00", align 1
@m = global i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0)
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarStruct)
{
    const char test_code[] = R"(
struct Point2D = x:f64, y:f64
var point:Point2D
)";
    const char *expected_ir = R"(
%Point2D = type { double, double }

@point = global %Point2D zeroinitializer
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}


TEST(testCGVar, testGlobalVarStructInitializer)
{
    const char test_code[] = R"(
struct Point2D = x:int, y:int
let point:Point2D = Point2D { 10, 20 }
)";
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

@point = global %Point2D { i32 10, i32 20 }
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}

TEST(testCGVar, testGlobalVarArrayInitializer)
{
    const char test_code[] = R"(
var a = [10]
)";
    const char *expected_ir = R"(
@a = global [1 x i32] [i32 10]
)";
    validate_m_code_with_ir_code(test_code, expected_ir);
}
