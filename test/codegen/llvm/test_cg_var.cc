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


TEST_F(TestFixture, testCGVarGlobalVarInt)
{
    const char test_code[] = "let m = 10";
    const char *expected_ir = R"(
@m = global i32 10
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarChar)
{
    const char test_code[] = "let m = 'm'";
    const char *expected_ir = R"(
@m = global i8 109
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarBool)
{
    const char test_code[] = "let m = True";
    const char *expected_ir = R"(
@m = global i8 1
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarDouble)
{
    const char test_code[] = "let m = 10.0";
    const char *expected_ir = R"(
@m = global double 1.000000e+01
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarString)
{
    const char test_code[] = R"(let m = "10.0")";
    const char *expected_ir = R"(
@0 = private unnamed_addr constant [5 x i8] c"10.0\00", align 1
@m = global ptr @0
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarStruct)
{
    const char test_code[] = R"(
struct Point2D = x:f64, y:f64
let point:Point2D
)";
    const char *expected_ir = R"(
%Point2D = type { double, double }

@point = global %Point2D zeroinitializer
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}


TEST_F(TestFixture, testCGVarGlobalVarStructInitializer)
{
    const char test_code[] = R"(
struct Point2D = x:int, y:int
let point:Point2D = Point2D { 10, 20 }
)";
    const char *expected_ir = R"(
%Point2D = type { i32, i32 }

@point = global %Point2D { i32 10, i32 20 }
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

TEST_F(TestFixture, testCGVarGlobalVarArrayInitializer)
{
    const char test_code[] = R"(
let a = [10]
)";
    const char *expected_ir = R"(
@a = global [1 x i32] [i32 10]
)";
    validate_m_code_with_ir_code(engine, test_code, expected_ir);
}

// TEST_F(TestFixture, testCGVarnew_del)
// {
//     char test_code[] = "\n\
// struct Point = x:f64, y:f64\n\
// let xy = new Point{100.0, 200.0}\n\
// del xy";
//     const char *expected_ir = R"(
// @a = global [1 x i32] [i32 10]
// )";
//     validate_m_code_with_ir_code(engine, test_code, expected_ir);
// }
