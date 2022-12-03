/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm/cg_wasm.h"
#include "codegen/wasm/wasm_abi.h"
#include "codegen/wasm/wasm_api.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "parser/amodule.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include "sema/eval.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

const char wasm_magic_number[] = {0, 'a', 's', 'm'};
u8 wasm_version[] = {0x01, 0, 0, 0};
u8 type_2_const[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ OPCODE_I32CONST,
    /*CHAR*/ OPCODE_I32CONST,
    /*i8*/ OPCODE_I32CONST,
    /*u8*/ OPCODE_I32CONST,
    /*i16*/ OPCODE_I32CONST,
    /*u16*/ OPCODE_I32CONST,
    /*i32*/ OPCODE_I32CONST,
    /*u32*/ OPCODE_I32CONST,
    /*i64*/ OPCODE_I32CONST,
    /*u64*/ OPCODE_I32CONST,
    /*INT*/ OPCODE_I32CONST,
    /*F32*/ OPCODE_F32CONST,
    /*F64*/ OPCODE_F64CONST,
    /*STRING*/ OPCODE_I32CONST,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*ARRAY*/0,
    /*UNION*/ 0,
    /*COMPLEX*/ 0,
    /*REF*/ 0,
};

u8 type_2_wtype[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ WASM_TYPE_I32,
    /*UNIT*/ WASM_TYPE_VOID,
    /*BOOL*/ WASM_TYPE_I32,
    /*CHAR*/ WASM_TYPE_I32,
    /*i8*/ WASM_TYPE_I32,
    /*u8*/ WASM_TYPE_I32,
    /*i16*/ WASM_TYPE_I32,
    /*u16*/ WASM_TYPE_I32,
    /*i32*/ WASM_TYPE_I32,
    /*u32*/ WASM_TYPE_I32,
    /*i64*/ WASM_TYPE_I64,
    /*u64*/ WASM_TYPE_I64,
    /*INT*/ WASM_TYPE_I32,
    /*F32*/ WASM_TYPE_F32,
    /*F64*/ WASM_TYPE_F64,
    /*STRING*/ WASM_TYPE_I32,
    /*FUNCTION*/ 0,
    /*STRUCT*/ WASM_TYPE_I32,
    /*ARRAY*/ WASM_TYPE_I32,
    /*UNION*/ WASM_TYPE_I32,
    /*COMPLEX*/ WASM_TYPE_I32,
    /*REF*/ WASM_TYPE_I32,
};

u8 type_2_store_op[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ OPCODE_I32STORE,
    /*CHAR*/ OPCODE_I32STORE,
    /*i8*/ OPCODE_I32STORE_8,
    /*u8*/ OPCODE_I32STORE_8,
    /*i16*/ OPCODE_I32STORE_16,
    /*u16*/ OPCODE_I32STORE_16,
    /*i32*/ OPCODE_I32STORE,
    /*u32*/ OPCODE_I32STORE,
    /*i64*/ OPCODE_I64STORE,
    /*u64*/ OPCODE_I64STORE,
    /*INT*/ OPCODE_I32STORE,
    /*F32*/ OPCODE_F32STORE,
    /*F64*/ OPCODE_F64STORE,
    /*STRING*/ OPCODE_I32STORE,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*ARRAY*/ 0,
    /*UNION*/ 0,
    /*COMPLEX*/ 0,
    /*REF*/OPCODE_I32STORE
};

u8 type_2_load_op[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ OPCODE_I32LOAD,
    /*CHAR*/ OPCODE_I32LOAD,
    /*i8*/ OPCODE_I32LOAD_8S,
    /*u8*/ OPCODE_I32LOAD_8U,
    /*i16*/ OPCODE_I32LOAD_16S,
    /*u16*/ OPCODE_I32LOAD_16U,
    /*i32*/ OPCODE_I32LOAD,
    /*u32*/ OPCODE_I32LOAD,
    /*i64*/ OPCODE_I64LOAD,
    /*u64*/ OPCODE_I64LOAD,
    /*INT*/ OPCODE_I32LOAD,
    /*F32*/ OPCODE_F32LOAD,
    /*F64*/ OPCODE_F64LOAD,
    /*STRING*/ OPCODE_I32LOAD,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*ARRAY*/ 0,
    /*UNION*/ 0,
    /*COMPLEX*/ 0,
    /*REF*/OPCODE_I32LOAD
};

u8 type_conversion_op[TYPE_TYPES][TYPE_TYPES] = {
    //UNK, GENERIC, UNIT, BOOL, CHAR, i8, u8, i16, u16, i32, u32, i64, u64, INT, F32, F64, STRING, FUNCTION, STRUCT, UNION     
    /*UNK*/ {0},
    /*GENERIC*/ {0},
    /*UNIT*/ {0},
    /*BOOL*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*CHAR*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*i8*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*u8*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*i16*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*u16*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*i32*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32S, /*f64*/OPCODE_F64CONVERT_I32S},
    /*u32*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32U, /*f64*/OPCODE_F64CONVERT_I32U},
    /*i64*/ {0, 0, 0, 0},
    /*u64*/ {0, 0, 0, 0},
    /*INT*/ {0, 0, 0, /*BOOL*/0, /*CHAR*/0, /*i8*/0, /*u8*/0, /*i16*/0, /*u16*/0, /*i32*/0, /*u32*/0, /*i64*/0, /*u64*/0, /*int*/0, /*f32*/OPCODE_F32CONVERT_I32U, /*f64*/OPCODE_F64CONVERT_I32U},
    /*F32*/ {0, 0, 0, /*BOOL*/OPCODE_I32TRUNC_F32S, /*CHAR*/OPCODE_I32TRUNC_F32S, /*i8*/OPCODE_I32TRUNC_F32S, /*u8*/OPCODE_I32TRUNC_F32S, /*i16*/OPCODE_I32TRUNC_F32S, /*u16*/OPCODE_I32TRUNC_F32S, /*i32*/OPCODE_I32TRUNC_F32S, /*u32*/OPCODE_I32TRUNC_F32U, /*i64*/OPCODE_I64TRUNC_F32S, /*u64*/OPCODE_I64TRUNC_F32U, /*int*/OPCODE_I32TRUNC_F32S, /*f32*/0, /*f64*/OPCODE_F64PROMOTE_F32},
    /*F64*/ {0, 0, 0, /*BOOL*/OPCODE_I32TRUNC_F64S, /*CHAR*/OPCODE_I32TRUNC_F64S, /*i8*/OPCODE_I32TRUNC_F64S, /*u8*/OPCODE_I32TRUNC_F64S, /*i16*/OPCODE_I32TRUNC_F64S, /*u16*/OPCODE_I32TRUNC_F64S, /*i32*/OPCODE_I32TRUNC_F64S, /*u32*/OPCODE_I32TRUNC_F64U, /*i64*/OPCODE_I64TRUNC_F64S, /*u64*/OPCODE_I64TRUNC_F64U, /*int*/OPCODE_I32TRUNC_F64S, /*f32*/OPCODE_F32DEMOTE_F64, /*f64*/0},
    /*STRING*/ {0, 0, 0, 0},
    /*FUNCTION*/ {0},
    /*STRUCT*/ {0},
    /*ARRAY*/ {0},
    /*UNION*/ {0},
    /*COMPLEX*/ {0},
    /*REF*/{0}
};

u64 max_int_bits[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ 0xFF,
    /*CHAR*/ 0xFF,
    /*i8*/ 0xFF,
    /*u8*/ 0xFF,
    /*i16*/ 0xFFFF,
    /*u16*/ 0xFFFF,
    /*i32*/ 0xFFFFFFFF,
    /*u32*/ 0xFFFFFFFF,
    /*i64*/ 0xFFFFFFFFFFFFFFFF,
    /*u64*/ 0xFFFFFFFFFFFFFFFF,
    /*INT*/ 0xFFFFFFFF,
    /*F32*/ 0,
    /*F64*/ 0,
    /*STRING*/ 0,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*ARRAY*/ 0,
    /*UNION*/ 0,
    /*COMPLEX*/ 0,
    /*REF*/0
};

u64 extend_signs[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ 0,
    /*CHAR*/ OPCODE_I32EXTEND8_S,
    /*i8*/ OPCODE_I32EXTEND8_S,
    /*u8*/ 0,
    /*i16*/ OPCODE_I32EXTEND16_S,
    /*u16*/ 0,
    /*i32*/ 0,
    /*u32*/ 0,
    /*i64*/ 0,
    /*u64*/ 0,
    /*INT*/ 0,
    /*F32*/ 0,
    /*F64*/ 0,
    /*STRING*/ 0,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*ARRAY*/ 0,
    /*UNION*/ 0,
    /*COMPLEX*/ 0,
    /*REF*/0
};

u8 op_maps[OP_TOTAL][TYPE_TYPES] = {
    /*
    UNK, GENERIC, UNIT, BOOL, CHAR, i8, u8, i16, u16, i32, u32, i64, u64, INT, F32, F64, STRING, FUNCTION, STRUCT, UNION     
    */
    /*OP_NULL   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,},
    /*OP_OR     */{0, 0, 0, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, 0, 0, 0, 0, 0, 0,},
    /*OP_AND    */{0, 0, 0, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, 0, 0, 0, 0, 0, 0,},
    /*OP_NOT    */{0, 0, 0, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0,}, //xor 1

    /*OP_BITNOT   */{0, 0, 0, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0,},             //xor -1
    /*OP_BITOR    */{0, 0, 0, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, 0, 0, 0, 0, 0, 0,},
    /*OP_BITEXOR   */{0, 0, 0, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0,},
    /*OP_BAND   */{0, 0, 0, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, 0, 0, 0, 0, 0, 0,},
    /*OP_BSL    */{0, 0, 0, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, 0, 0, 0, 0, 0, 0,},
    /*OP_BSR    */{0, 0, 0, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, 0, 0, 0, 0, 0, 0,},

    /*OP_SQRT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, OPCODE_F32SQRT, OPCODE_F64SQRT, 0, 0, 0, 0,},
    /*OP_POW    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_STAR  */{0, 0, 0, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_F32MUL, OPCODE_F64MUL, 0, 0, 0, 0,},
    /*OP_DIV    */{0, 0, 0, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_F32DIV, OPCODE_F64DIV, 0, 0, 0, 0,},
    /*OP_MOD    */{0, 0, 0, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, 0, 0, 0, 0, 0, 0,},
    /*OP_PLUS   */{0, 0, 0, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_F32ADD, OPCODE_F64ADD, 0, 0, 0, 0,},
    /*OP_MINUS  */{0, 0, 0, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_F32SUB, OPCODE_F64SUB, 0, 0, 0, 0,},

    /*OP_LT   */{0, 0, 0, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_F32LT, OPCODE_F64LT, 0, 0, 0, 0,},
    /*OP_LE  */{0, 0, 0, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_F32LE, OPCODE_F64LE, 0, 0, 0, 0,},
    /*OP_EQ    */{0, 0, 0, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_F32EQ, OPCODE_F64EQ, 0, 0, 0, 0,},
    /*OP_GT    */{0, 0, 0, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_F32GT, OPCODE_F64GT, 0, 0, 0, 0,},
    /*OP_GE   */{0, 0, 0, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_F32GE, OPCODE_F64GE, 0, 0, 0, 0,},
    /*OP_NE  */{0, 0, 0, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_F32NE, OPCODE_F64NE, 0, 0, 0, 0,},

    /*OP_COND  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},

    /*OP_MUL_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DIV_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_MOD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_ADD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_SUB_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_LEFT_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_RIGHT_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_AND_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_XOR_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_OR_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},

    /*OP_INC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DEC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
} ;


#define DATA_SECTION_START_ADDRESS 1024
#define STACK_BASE_ADDRESS  66592

#define MEMORY_BASE_VAR_INDEX 1

symbol MEMORY = 0;
symbol __MEMORY_BASE = 0;
symbol POW_FUN_NAME = 0;

void _imports_init(struct imports *imports)
{
    imports->import_block = 0;
    imports->num_fun = 0;
    imports->num_global = 0;
    imports->num_memory = 0;
}

void _imports_deinit(struct imports *imports)
{
    node_free(imports->import_block);
    imports->import_block = 0;
    imports->num_fun = 0;
    imports->num_global = 0;
    imports->num_memory = 0;
}

void _cg_wasm_init(struct cg_wasm *cg)
{
    ba_init(&cg->ba, 17);
    hashtable_init_with_value_size(&cg->func_name_2_idx, sizeof(u32), 0);
    hashtable_init(&cg->func_name_2_ast);
    _imports_init(&cg->imports);
    cg->sys_block = 0;
    cg->fun_top = 0;
    cg->var_top = 0;
    cg->func_idx = 0;
    cg->data_offset = 0;
    cg->fun_types = block_node_new_empty();
    cg->funs = block_node_new_empty();
    cg->data_block = block_node_new_empty();
}

void _cg_wasm_deinit(struct cg_wasm *cg)
{
    _imports_deinit(&cg->imports);
    node_free(cg->sys_block);
    cg->fun_top = 0;
    hashtable_deinit(&cg->func_name_2_ast);
    hashtable_deinit(&cg->func_name_2_idx);
    ba_deinit(&cg->ba);
    free_block_node(cg->fun_types, false); //container only
    free_block_node(cg->funs, false); //container only
    free_block_node(cg->data_block, false); //container only
}

void wasm_emit_store_scalar_value_at(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 align, u32 offset, struct ast_node *node)
{
    wasm_emit_get_var(ba, local_address_var_index, false); 
    wasm_emit_store_scalar_value(cg, ba, align, offset, node);
}

void wasm_emit_store_scalar_value(struct cg_wasm *cg, struct byte_array *ba, u32 align, u32 offset, struct ast_node *node)
{
    wasm_emit_code(cg, ba, node);  
    wasm_emit_store_mem(ba, align, offset, node->type->type);
}

void wasm_emit_var_change(struct cg_wasm *cg, WasmModule ba, u32 var_index, bool is_global, u8 op, u32 elm_size, struct ast_node* offset_index)
{
    wasm_emit_get_var(ba, var_index, is_global);
    //
    wasm_emit_code(cg, ba, offset_index);
    wasm_emit_const_i32(ba, elm_size);
    ba_add(ba, OPCODE_I32MUL);

    ba_add(ba, op); 
}

void wasm_emit_addr_offset_by_expr(struct cg_wasm *cg, WasmModule ba, u32 var_index, bool is_global, struct ast_node *offset_expr)
{
    wasm_emit_get_var(ba, var_index, is_global);
    wasm_emit_code(cg, ba, offset_expr);
    ba_add(ba, OPCODE_I32ADD);
}

TargetType _get_function_type(TargetType ret_type, TargetType *param_types, unsigned param_count, bool is_variadic)
{
    return 0;
}

TargetType _get_target_type(struct type_expr *type)
{
    return &type_2_wtype[type->type];
}

TargetType _get_pointer_type(TargetType type)
{
    return &type_2_wtype[TYPE_INT];
}

TargetType _get_size_int_type(unsigned width)
{
    return &type_2_wtype[TYPE_INT];
}


void _init_target_info(struct target_info *ti)
{
    ti->extend_type = &type_2_wtype[TYPE_INT]; //would use 32 bits
    ti->get_size_int_type = _get_size_int_type;
    ti->get_pointer_type = _get_pointer_type; //LLVMPointerType(get_llvm_type(fi->ret.type), 0)
    ti->get_target_type = _get_target_type; //get_llvm_type(fi->ret.type)
    ti->get_function_type = _get_function_type;
    ti->fill_struct_fields = 0;//
    ti->get_count_struct_element_types = 0; //LLVMCountStructElementTypes
    ti->void_type = &type_2_wtype[TYPE_UNIT];
}
struct cg_wasm *cg_wasm_new(struct sema_context *context)
{
    struct cg_wasm *cg;
    MALLOC(cg, sizeof(*cg));
    _cg_wasm_init(cg);
    MEMORY = to_symbol("memory");
    __MEMORY_BASE = to_symbol("__memory_base");
    POW_FUN_NAME = to_symbol("pow");
    cg->base.compute_fun_info = wasm_compute_fun_info;
    cg->base.sema_context = context;
    cg->base.target_info = ti_new("wasm32");
    _init_target_info(cg->base.target_info);
    return cg;
}

void cg_wasm_free(struct cg_wasm *cg)
{
    _cg_wasm_deinit(cg);
    ti_free(cg->base.target_info);
    free(cg);
}

struct fun_context *cg_get_top_fun_context(struct cg_wasm *cg)
{
    return cg->fun_top >= 1 ? &cg->fun_contexts[cg->fun_top - 1] : 0;
}

void _emit_literal(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->type && node->type->type < TYPE_TYPES && node->type->type >= 0);
    u32 len;
    switch(node->type->type){
        default:
            printf("unknown type: %s\n", string_get(get_type_symbol(node->type->type)));
            break;
        case TYPE_BOOL:
        case TYPE_CHAR:
        case TYPE_I8:
        case TYPE_U8:
        case TYPE_I16:
        case TYPE_U16:
        case TYPE_I32:
        case TYPE_U32:
        case TYPE_INT:
            wasm_emit_const_i32(ba, node->liter->int_val);
            break;
        case TYPE_F32:
            wasm_emit_const_f32(ba, node->liter->double_val);
            break;
        case TYPE_F64:
            wasm_emit_const_f64(ba, node->liter->double_val);
            break;
        case TYPE_STRING:
            len = strlen(node->liter->str_val);
            if(cg->imports.num_memory){
                ba_add(ba, OPCODE_GLOBALGET);
                wasm_emit_uint(ba, MEMORY_BASE_VAR_INDEX);
                if(cg->data_offset){
                    wasm_emit_const_i32(ba, cg->data_offset);
                    ba_add(ba, OPCODE_I32ADD);
                }
                cg->data_offset += len + 1; //null terminated string
            } else {
                ba_add(ba, type_2_const[node->type->type]);
                wasm_emit_uint(ba, DATA_SECTION_START_ADDRESS + cg->data_offset);
                cg->data_offset += wasm_get_emit_size(len) + len;
            }
            block_node_add(cg->data_block, node);
            break;
    }
    
}

void _emit_unary(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct ast_node *new_node = 0;
    symbol s = 0;
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct var_info *vi;
    switch (node->unop->opcode){
        default:
            s = get_terminal_symbol_by_token_opcode(TOKEN_OP, node->unop->opcode);
            printf("Not implemented unary for : %s\n", string_get(s));
            break;
        case OP_STAR:
            if(!node->is_lvalue){
                //rvalue, for reading
                if(!is_aggregate_type(node->type)){
                    //read from memory
                    assert(node->unop->operand->node_type == IDENT_NODE);
                    vi = fc_get_var_info(fc, node->unop->operand->ident->var);
                    u32 align = get_type_align(node->type);
                    wasm_emit_load_mem_from(ba, vi->var_index, false, align, 0, node->type->type);
                }
            }else{
                //write to the memory
            }
            break;
        case OP_BAND: //reference returning the address of variable
            if(node->unop->operand->node_type == IDENT_NODE){
                //emit the location of the var definition
                vi = fc_get_var_info(fc, node->unop->operand->ident->var);
                wasm_emit_addr_offset(ba, vi->var_index, false, 0);
            }else{
                wasm_emit_code(cg, ba, node->unop->operand);
                vi = fc_get_var_info(fc, node->unop->operand);
                wasm_emit_addr_offset(ba, vi->var_index, false, 0);
            }
            break;
        case OP_SQRT:
        {
            enum type type_index = prune(node->unop->operand->type)->type;
            wasm_emit_code(cg, ba, node->unop->operand);
            ba_add(ba, op_maps[node->unop->opcode][type_index]);
            break;
        }
        case OP_MINUS:
            new_node = int_node_new(0, node->loc);
            new_node->type = node->type;
            wasm_emit_code(cg, ba, new_node);
            break;
        case OP_NOT:
            new_node = int_node_new(1, node->loc);
            new_node->type = node->type;
            wasm_emit_code(cg, ba, new_node);
            break;
        case OP_BITNOT:
            new_node = int_node_new(-1, node->loc);
            new_node->type = node->type;
            wasm_emit_code(cg, ba, new_node);
            break;
        }
    if (new_node) {
        wasm_emit_code(cg, ba, node->unop->operand);
        enum type type_index = prune(node->unop->operand->type)->type;
        assert(type_index >= 0 && type_index < TYPE_TYPES);
        assert(node->unop->opcode >= 0 && node->unop->opcode < OP_TOTAL);
        u8 opcode = op_maps[node->unop->opcode][type_index];
        if(!opcode){
            symbol s = get_terminal_symbol_by_token_opcode(TOKEN_OP, node->unop->opcode);
            printf("No opcode found for op: %s, type: %s\n", string_get(s), string_get(get_type_symbol(type_index)));
        }else{
            ba_add(ba, opcode);
            ast_node_free(new_node);
        }
    }
}

void _emit_cast(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    wasm_emit_code(cg, ba, node->cast->expr);
    if(node->type->type != node->cast->expr->type->type){
        u8 op = type_conversion_op[node->cast->expr->type->type][node->type->type];
        if(!op){
            if(node->type->type < node->cast->expr->type->type){
                //downcasting for int, cut bits
                u64 bit_mask = max_int_bits[node->type->type];
                assert(bit_mask);
                wasm_emit_const_i32(ba, bit_mask);
                ba_add(ba, OPCODE_I32AND);
                //we need to upcasting to i32/i64 since WASM only supports i32/i64 int
                u8 sign_op = extend_signs[node->type->type];
                if(sign_op){
                    ba_add(ba, sign_op);
                }
            }else{
                //up casting from expr type
                u8 sign_op = extend_signs[node->cast->expr->type->type];
                if(sign_op){
                    ba_add(ba, sign_op);
                }
            }
        }else{
            ba_add(ba, op);
        }
    }
}

void _free_field_info(void *fi)
{
    struct field_info *field = fi;
    node_free(field->offset_expr);
}

void _emit_struct_field_accessor(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    //lhs is struct var, rhs is field var name
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct array field_infos;
    array_init_free(&field_infos, sizeof(struct field_info), _free_field_info);
    sc_get_field_infos_from_root(cg->base.sema_context, node, &field_infos);
    struct field_info *field = array_front(&field_infos);
    struct var_info*root_vi = fc_get_var_info(fc, field->aggr_root);
    u32 offset = eval(field->offset_expr);
    wasm_emit_code(cg, ba, field->aggr_root);
    if (is_aggregate_type(field->type)){
        if(node->is_ret){
            //copy result into variable index 0
            //for sret
            wasm_emit_copy_struct_value(ba, 0, 0, field->type, root_vi->var_index, offset);
        }
        //calculate new address and push it to stack
        //wasm_emit_change_var(ba, OPCODE_I32ADD, field.offset, root_vi->var_index, false);
    }else{ //scalar value
        //read the value: scalar value read
        if(!node->is_lvalue){//read for right value
            wasm_emit_load_mem_from(ba, root_vi->var_index, false, field->align, offset, field->type->type);
        }
    }
    array_deinit(&field_infos);
}

struct ast_node *_get_root_array(struct ast_node *node)
{
    while(node->node_type == MEMBER_INDEX_NODE && node->index->aggregate_type == AGGREGATE_TYPE_ARRAY){
        node = node->index->object;
    }
    return node;
}

void _emit_array_member_accessor(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    //lhs is array, rhs is int expression
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct array field_infos;
    array_init_free(&field_infos, sizeof(struct field_info), _free_field_info);
    sc_get_field_infos_from_root(cg->base.sema_context, node, &field_infos);
    struct field_info *field = array_front(&field_infos); //assuming only one element in the field info
    struct var_info*root_vi = fc_get_var_info(fc, field->aggr_root);
    wasm_emit_code(cg, ba, field->aggr_root);
    if (is_aggregate_type(field->type)){
        if(node->is_ret){
            //copy result into variable index 0
            //for sret
            //wasm_emit_copy_struct_value(ba, 0, 0, field_type, root_vi->var_index, field->offset);
        }
        //calculate new address and push it to stack
        //wasm_emit_change_var(ba, OPCODE_I32ADD, field.offset, root_vi->var_index, false);
    }else{ //scalar value
        //read the value: scalar value read
        if(!node->is_lvalue){//read for right value
            wasm_emit_addr_offset_by_expr(cg, ba, root_vi->var_index, false, field->offset_expr);
            wasm_emit_load_mem(ba, field->align, 0, field->type->type);
        }
    }
}

void _emit_assign_value(struct cg_wasm *cg, struct byte_array *ba, u32 addr_var_index, u32 offset, u32 align, struct type_expr *type, struct ast_node *rhs)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    if(type->type == TYPE_STRUCT){
        //struct assignment/copy
        struct var_info *rhs_vi = fc_get_var_info(fc, rhs);
        wasm_emit_code(cg, ba, rhs); 
        wasm_emit_copy_struct_value(ba, addr_var_index, offset, rhs->type, rhs_vi->var_index, 0);
    } else {
        //scalar version
        wasm_emit_store_scalar_value_at(cg, ba, addr_var_index, align, offset, rhs);
    }    
}

void _emit_assign_struct_field(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *lhs, struct ast_node *rhs)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct array field_infos;
    array_init_free(&field_infos, sizeof(struct field_info), _free_field_info);
    sc_get_field_infos_from_root(cg->base.sema_context, lhs, &field_infos);
    struct field_info *field = array_front(&field_infos);
    struct var_info*vi = fc_get_var_info(fc, field->aggr_root);
    u32 offset = eval(field->offset_expr);
    _emit_assign_value(cg, ba, vi->var_index, offset, field->align, field->type, rhs);
    array_deinit(&field_infos);
}

void _emit_assign_array_field(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *lhs, struct ast_node *rhs)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    struct array field_infos;
    array_init_free(&field_infos, sizeof(struct field_info), _free_field_info);
    sc_get_field_infos_from_root(cg->base.sema_context, lhs, &field_infos);
    struct field_info *field = array_front(&field_infos);
    struct var_info*vi = fc_get_var_info(fc, field->aggr_root);
    wasm_emit_addr_offset_by_expr(cg, ba, vi->var_index, false, field->offset_expr);
    wasm_emit_store_scalar_value(cg, ba, field->align, 0, rhs);
}

void _emit_assignment(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct ast_node *lhs = node->binop->lhs;
    struct fun_context *fc = cg_get_top_fun_context(cg);
    
    if(lhs->node_type == IDENT_NODE && !lhs->ident->var->is_addressed && !is_aggregate_type(lhs->type)){
        //simple scalar version, no memory load/store op
        struct var_info*vi = fc_get_var_info(fc, lhs);
        wasm_emit_code(cg, ba, node->binop->rhs);
        wasm_emit_set_var(ba, vi->var_index, false);
        return;
    }

    wasm_emit_code(cg, ba, node->binop->lhs); 
    if(node->binop->lhs->node_type == MEMBER_INDEX_NODE){
        if(node->binop->lhs->index->aggregate_type == AGGREGATE_TYPE_ARRAY){
            _emit_assign_array_field(cg, ba, node->binop->lhs, node->binop->rhs);
        } else if(node->binop->lhs->index->aggregate_type == AGGREGATE_TYPE_STRUCT){
            _emit_assign_struct_field(cg, ba, node->binop->lhs, node->binop->rhs);
        }
    }else{
        struct var_info*vi = fc_get_var_info(fc, lhs);
        u32 align = get_type_align(lhs->type);
        _emit_assign_value(cg, ba, vi->var_index, 0, align, lhs->type, node->binop->rhs);
    }
}

void _emit_binary(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    wasm_emit_code(cg, ba, node->binop->lhs); 
    wasm_emit_code(cg, ba, node->binop->rhs); 
    struct type_expr *lhs_type = node->binop->lhs->transformed ? node->binop->lhs->transformed->type : node->binop->lhs->type;
    enum type type_index = prune(lhs_type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->binop->opcode >= 0 && node->binop->opcode < OP_TOTAL);
    //if left side is aggregate type, then it is to store the right value to the left address
    if (node->binop->opcode != OP_POW){
        u8 opcode = op_maps[node->binop->opcode][type_index];
        if(!opcode){
            symbol s = get_terminal_symbol_by_token_opcode(TOKEN_OP, node->binop->opcode);
            printf("No opcode found for op: %s, type: %s\n", string_get(s), string_get(get_type_symbol(type_index)));
        }else{
            ba_add(ba, opcode);
        }
    }else{
        //call pow function
        u32 func_index = hashtable_get_int(&cg->func_name_2_idx, POW_FUN_NAME);
        ba_add(ba, OPCODE_CALL);
        wasm_emit_uint(ba, func_index);
    }
}

bool is_variadic_call_with_optional_arguments(struct cg_wasm *cg, struct ast_node *node)
{
    struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, node->call->specialized_callee ? node->call->specialized_callee : node->call->callee);
    return fun_type->ft->is_variadic && array_size(&node->call->arg_block->block->nodes) >= array_size(&fun_type->ft->params->block->nodes);
}

void _emit_const_zero(struct byte_array* ba, enum  type type)
{
    if(is_int_type(type)){
        wasm_emit_const_i32(ba, 0);
    }else{
        wasm_emit_const_f64(ba, 0.0);
    }
}

void _emit_if(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == IF_NODE);
    wasm_emit_code(cg, ba, node->cond->if_node);
    ba_add(ba, OPCODE_IF);
    ASSERT_TYPE(node->cond->then_node->type->type);
    ba_add(ba, type_2_wtype[node->cond->then_node->type->type]);
    wasm_emit_code(cg, ba, node->cond->then_node);
    if (node->cond->else_node) {
        ba_add(ba, OPCODE_ELSE);
        wasm_emit_code(cg, ba, node->cond->else_node);
    }
    ba_add(ba, OPCODE_END);
}

void _emit_case_block(struct cg_wasm *cg, struct byte_array *ba, struct byte_array *embeded_blocks, struct ast_node *node, u32 nested_levels)
{
    ba_add(ba, OPCODE_BLOCK); 
    ba_add(ba, type_2_wtype[node->type->type]); 
    ba_add2(ba, embeded_blocks);
    wasm_emit_code(cg, ba, node);
    ba_add(ba, OPCODE_BR);
    wasm_emit_uint(ba, nested_levels);
    ba_add(ba, OPCODE_END);
}

void _emit_jump_table(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *test_expr, u32 size)
{
    ba_add(ba, OPCODE_BLOCK);
    //ba_add(ba, WASM_TYPE_VOID);
    ba_add(ba, type_2_wtype[TYPE_INT]);
    wasm_emit_const_i32(ba, 0); //return 
    wasm_emit_code(cg, ba, test_expr);
    ba_add(ba, OPCODE_BR_TABLE);
    wasm_emit_uint(ba, size); //index vector size
    for(u32 j = 0; j < size; j++){
        wasm_emit_uint(ba, j); //index element
    }
    wasm_emit_uint(ba, size); //default branch
    ba_add(ba, OPCODE_END);
}

void _emit_match(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    //
    struct byte_array cases_ba, case_ba;
    ba_init(&cases_ba, 17);
    ba_init(&case_ba, 17);
    
    u32 num_case = array_size(&node->match->match_cases->block->nodes);
    _emit_jump_table(cg, &cases_ba, node->match->test_expr, num_case);
    for(size_t i = 0; i < array_size(&node->match->match_cases->block->nodes); i++){
        struct ast_node *case_node = *(struct ast_node **)array_get(&node->match->match_cases->block->nodes, i);
        ba_reset(&case_ba);
        _emit_case_block(cg, &case_ba, &cases_ba, case_node->match_case->expr, num_case - i);
        //copy 
        ba_reset(&cases_ba);
        ba_add2(&cases_ba, &case_ba);
    }
    
    //add default block
    {
        struct ast_node *default_node = int_node_new(0, node->loc);
        ba_reset(&case_ba);
        _emit_case_block(cg, &case_ba, &cases_ba, default_node, 0);
        //copy 
        ba_reset(&cases_ba);
        ba_add2(&cases_ba, &case_ba);
    }
    ba_add2(ba, &cases_ba);
    ba_deinit(&cases_ba);
    ba_deinit(&case_ba);
}

void _emit_if_local_var_ge_zero(struct byte_array *ba, u32 var_index, enum type type)
{
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    _emit_const_zero(ba, type);
    ba_add(ba, op_maps[OP_GE][type]);
    ba_add(ba, OPCODE_IF);
    ba_add(ba, WASM_TYPE_I32);
}

void _emit_for_loop(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct fun_context *fc = cg_get_top_fun_context(cg);
    assert(node->node_type == FOR_NODE);
    u32 var_index = fc_get_var_info(fc, node->forloop->var)->var_index;
    u32 step_index = fc_get_var_info(fc, node->forloop->range->range->step)->var_index;
    u32 end_index = fc_get_var_info(fc, node->forloop->range->range->end->binop->rhs)->var_index;
    enum type type = node->forloop->var->type->type;
    //enum type body_type = node->forloop->body->type->type;
    ASSERT_TYPE(type);
    //ASSERT_TYPE(body_type);
    // initializing start value
    wasm_emit_code(cg, ba, node->forloop->range->range->start);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, var_index);  //1

    // set step value
    wasm_emit_code(cg, ba, node->forloop->range->range->step);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, step_index); //2
    // set end value
    assert(node->forloop->range->range->end->node_type == BINARY_NODE);
    wasm_emit_code(cg, ba, node->forloop->range->range->end->binop->rhs);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, end_index);  //3

    ba_add(ba, OPCODE_BLOCK); // outside block branch labelidx 1
    ba_add(ba, WASM_TYPE_VOID); // type_2_wtype[body_type]); // branch type

    ba_add(ba, OPCODE_LOOP);  // loop branch, branch labelidx 0
    ba_add(ba, WASM_TYPE_VOID); //type_2_wtype[body_type]); // branch type

    //if step >= 0
    _emit_if_local_var_ge_zero(ba, step_index, node->forloop->range->range->step->type->type);
    //branch body
    //1. get var value
    //nested a if branch
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, end_index);
    ba_add(ba, op_maps[OP_GE][type]);

    ba_add(ba, OPCODE_ELSE);

    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, end_index);
    ba_add(ba, op_maps[OP_LE][type]);
    ba_add(ba, OPCODE_END); 
    //end of if step >= 0

    ba_add(ba, OPCODE_BR_IF); //if out of scope, branch to out side block
    wasm_emit_uint(ba, 1); //if true, jump out of block

    //body, another block into for loop due to post loop adjustment

    ba_add(ba, OPCODE_BLOCK); // add body block
    ba_add(ba, WASM_TYPE_VOID); // void type
    wasm_emit_code(cg, ba, node->forloop->body);
    ba_add(ba, OPCODE_END); //end of body block

    //var += step
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, step_index);
    ba_add(ba, op_maps[OP_PLUS][type]);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_BR); //branch to loop again
    wasm_emit_uint(ba, 0); 
    ba_add(ba, OPCODE_END); //end of loop branch
    ba_add(ba, OPCODE_END); //end of outside branch
}

void _emit_while_loop(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    ba_add(ba, OPCODE_BLOCK); // outside block branch labelidx 1
    ba_add(ba, WASM_TYPE_VOID); // type_2_wtype[body_type]); // branch type

    ba_add(ba, OPCODE_LOOP);  // loop branch, branch labelidx 0
    ba_add(ba, WASM_TYPE_VOID); //type_2_wtype[body_type]); // branch type

    //if not true of node->whileloop->expr, then break the loop branch (goto outside block branch)
    struct ast_node *not_expr = unary_node_new(OP_NOT, node->whileloop->expr, false, node->whileloop->expr->loc);
    not_expr->type = node->whileloop->expr->type;
    wasm_emit_code(cg, ba, not_expr);
    ba_add(ba, OPCODE_BR_IF);
    wasm_emit_uint(ba, 1); //if true, jump out of loop branch, got to the outside block branch

    //body, this is to be consist to for loop, actually is not needed, but then break needs to be
    //aware of which loop it is in
    ba_add(ba, OPCODE_BLOCK); 
    ba_add(ba, WASM_TYPE_VOID); 
    wasm_emit_code(cg, ba, node->whileloop->body);
    ba_add(ba, OPCODE_END); //end of loop branch

    ba_add(ba, OPCODE_BR); //goto loop branch again
    wasm_emit_uint(ba, 0); 
    ba_add(ba, OPCODE_END); //end of loop branch
    ba_add(ba, OPCODE_END); //end of outside branch
    ast_node_free(not_expr);
}

void _emit_jump(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    if(node->jump->expr){
        wasm_emit_code(cg, ba, node->jump->expr);
    }
    if(node->jump->token_type == TOKEN_RETURN){
        ba_add(ba, OPCODE_RETURN);
    } else if(node->jump->token_type == TOKEN_CONTINUE){
        ba_add(ba, OPCODE_BR);
        wasm_emit_uint(ba, node->jump->nested_block_levels - 1); //jump to innermost loop
    } else if(node->jump->token_type == TOKEN_BREAK){
        ba_add(ba, OPCODE_BR);
        wasm_emit_uint(ba, node->jump->nested_block_levels + 1); //jump to out side of innermost loop
    }
}

void _emit_ident(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == IDENT_NODE);
    struct fun_context *fc = cg_get_top_fun_context(cg);
    u32 var_index = fc_get_var_info(fc, node)->var_index;
    if (is_ref_type(node->type) && is_aggregate_type(node->type->val_type)){
        //read y.z here node is y, field accessor will access independently 
        if(!node->ident->is_member_index_object){
            wasm_emit_get_var(ba, var_index, false);
        }
    }else if(is_aggregate_type(node->type)){
        //for aggregate data
        if(node->is_ret){
            wasm_emit_copy_struct_value(ba, 0, 0, node->type, var_index, 0);
        }
    }else{
        if(node->ident->var->is_addressed){
            //read value from memory
            u32 align = get_type_align(node->type);
            wasm_emit_load_mem_from(ba, var_index, false, align, 0, node->type->type);
        }else{
            wasm_emit_get_var(ba, var_index, false);
        }
    }
}

void _emit_block(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{   
    u32 block_size = array_size(&node->block->nodes);
    for(u32 i = 0; i < block_size; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->block->nodes, i);
        wasm_emit_code(cg, ba, child);
    }
}

void wasm_emit_code(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    if (node->transformed)
        node = node->transformed;
    switch(node->node_type){
        case FUNC_NODE:
            wasm_emit_func(cg, ba, node);
            break;
        case BLOCK_NODE:
            _emit_block(cg, ba, node);
            break;
        case MEMBER_INDEX_NODE:
            if(node->index->aggregate_type == AGGREGATE_TYPE_ARRAY){
                _emit_array_member_accessor(cg, ba, node);
            } else if(node->index->aggregate_type == AGGREGATE_TYPE_STRUCT){
                _emit_struct_field_accessor(cg, ba, node);
            }
            break;
        case BINARY_NODE:
            _emit_binary(cg, ba, node);
            break;
        case ASSIGN_NODE:
            _emit_assignment(cg, ba, node);
            break;
        case UNARY_NODE:
            _emit_unary(cg, ba, node);
            break;
        case CAST_NODE:
            _emit_cast(cg, ba, node);
            break;
        case LITERAL_NODE:
            _emit_literal(cg, ba, node);
            break;
        case CALL_NODE:
            wasm_emit_call(cg, ba, node);
            break;
        case IDENT_NODE:
            _emit_ident(cg, ba, node);
            break;
        case VAR_NODE:
            wasm_emit_var(cg, ba, node);
            break;
        case IF_NODE:
            _emit_if(cg, ba, node);
            break;
        case MATCH_NODE:
            _emit_match(cg, ba, node);
            break;
        case FOR_NODE:
            _emit_for_loop(cg, ba, node);
            break;
        case WHILE_NODE:
            _emit_while_loop(cg, ba, node);
            break;
        case JUMP_NODE:
            _emit_jump(cg, ba, node);
            break;
        case ADT_INIT_NODE:
            wasm_emit_adt_init(cg, ba, node);
            break;
        case ARRAY_INIT_NODE:
            wasm_emit_array_init(cg, ba, node);
            break;
        case MATCH_CASE_NODE:
        case UNION_NODE:
        case UNION_TYPE_ITEM_NODE:
        case ARRAY_TYPE_NODE:
        case TYPE_NODE:
        case STRUCT_NODE:
        case NULL_NODE:
        case UNIT_NODE:
        case IMPORT_NODE:
        case MEMORY_NODE:
        case RANGE_NODE:
        case FUNC_TYPE_NODE:
        case TOTAL_NODE:
            break;
    }
}

void _append_section(struct byte_array *ba, struct byte_array *section)
{
    wasm_emit_uint(ba, section->size); // set size
    ba_add2(ba, section); // copy data
    ba_reset(section);
}

void _emit_type_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 func_types = array_size(&block->block->nodes);
    wasm_emit_uint(ba, func_types);
    struct ast_node *func_type_node;
    u32 i, j;
    u32 pi; /*param index*/
    struct type_expr *te;
    for (i = 0; i < func_types; i++) {
        func_type_node = *(struct ast_node **)array_get(&block->block->nodes, i);
        struct type_expr *func_type = func_type_node->type;
        u32 num_params = array_size(&func_type->args) - 1;
        struct fun_info *fi = compute_target_fun_info(cg->base.target_info, cg->base.compute_fun_info, func_type_node);
        bool has_sret = fi_has_sret(fi);
        ba_add(ba, TYPE_FUNC);
        if(has_sret){
            num_params += 1;
        }
        wasm_emit_uint(ba, num_params); // num params
        for (j = 0; j < num_params; j++) {
            pi = j;
            if(has_sret){
                if(j) pi = j - 1;
                else pi = num_params - 1;
            }
            te = *(struct type_expr **)array_get(&func_type->args, pi);
            ASSERT_TYPE(te->type);
            ba_add(ba, type_2_wtype[te->type]);
        }
        te = *(struct type_expr **)array_back(&func_type->args);
        ASSERT_TYPE(te->type);
        if (te->type == TYPE_UNIT||has_sret) {
            ba_add(ba, 0); // num result
        } else {
            ba_add(ba, 1); // num result
            ba_add(ba, type_2_wtype[te->type]); // i32 output
        }
    }
}

void _emit_import_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block) 
{
    u32 num_imports = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_imports); // number of imports
    u32 type_index = 0;
    for(u32 i = 0; i < array_size(&block->block->nodes); i++){
        struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, i);
        assert(node->node_type == IMPORT_NODE);
        wasm_emit_string(ba, node->import->from_module);
        node = node->import->import;
        switch(node->node_type){
        default:
            printf("%s node is not allowed in import section", node_type_strings[node->node_type]);
            break;
        case FUNC_TYPE_NODE:
            wasm_emit_string(ba, node->ft->name);
            ba_add(ba, IMPORT_FUNC);
            wasm_emit_uint(ba, type_index++); //type index
            break;
        case VAR_NODE:
            wasm_emit_string(ba, node->var->var->ident->name);
            ba_add(ba, IMPORT_GLOBAL);
            ASSERT_TYPE(node->type->type);
            ba_add(ba, type_2_wtype[node->type->type]);
            if (__MEMORY_BASE == node->var->var->ident->name)
                ba_add(ba, GLOBAL_CONST); // immutable
            else{
                ba_add(ba, GLOBAL_VAR); // mutable
            }
            break;
        case MEMORY_NODE:
            wasm_emit_string(ba, MEMORY);
            ba_add(ba, IMPORT_MEMORY);
            if(node->memory->max){
                ba_add(ba, LIMITS_MIN_MAX);
                wasm_emit_uint(ba, node->memory->initial->liter->int_val);
                wasm_emit_uint(ba, node->memory->max->liter->int_val);
            }else{
                ba_add(ba, LIMITS_MIN_ONLY);
                wasm_emit_uint(ba, node->memory->initial->liter->int_val);
            }
            break;
        }
    }
}

void _emit_function_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    ba_add(ba, num_func); // num functions
    for (u32 i = 0; i < num_func; i++) {
        wasm_emit_uint(ba, i + cg->imports.num_fun); // function index
    }
}

void _emit_memory_section(struct cg_wasm *cg, struct byte_array *ba)
{
    ba_add(ba, 1); // num memories
    ba_add(ba, LIMITS_MIN_MAX);
    ba_add(ba, 2);//min 2x64k
    ba_add(ba, 10);//max 10x64k
}

void _emit_global_section(struct cg_wasm *cg, struct byte_array *ba)
{
    //__stack_pointer: base address
    ba_add(ba, 1);  //num globals
    ba_add(ba, WASM_TYPE_I32);
    ba_add(ba, GLOBAL_VAR); //mutable
    wasm_emit_const_i32(ba, STACK_BASE_ADDRESS);
    ba_add(ba, OPCODE_END);
}

void _emit_export_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_func + 1); // num of function exports plus 1 memory
    struct ast_node *func;
    for (u32 i = 0; i < num_func; i++) {
        func = *(struct ast_node **)array_get(&block->block->nodes, i);
        wasm_emit_string(ba, func->func->func_type->ft->name);
        ba_add(ba, EXPORT_FUNC);
        wasm_emit_uint(ba, i + cg->imports.num_fun); // func index
    }
    wasm_emit_string(ba, MEMORY);
    ba_add(ba, EXPORT_MEMORY);
    wasm_emit_uint(ba, 0); //export memory 0
}

void _emit_code_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_func); // num functions
    wasm_emit_code(cg, ba, block);
}

void _emit_data_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_data = array_size(&block->block->nodes);
    wasm_emit_uint(ba, 1); //1 data segment
    wasm_emit_uint(ba, DATA_ACTIVE);
    // offset of memory
    if (cg->imports.num_memory){
        ba_add(ba, OPCODE_GLOBALGET);
        wasm_emit_uint(ba, MEMORY_BASE_VAR_INDEX);
    }else{
        ba_add(ba, OPCODE_I32CONST);
        wasm_emit_uint(ba, DATA_SECTION_START_ADDRESS);
    }
    ba_add(ba, OPCODE_END);
    wasm_emit_uint(ba, cg->data_offset);
    for (u32 i = 0; i < num_data; i++) {
        struct ast_node *node = *(struct ast_node**)array_get(&block->block->nodes, i);
        assert(node->node_type == LITERAL_NODE);
        //data array size and content
        u32 str_length = strlen(node->liter->str_val);
        if (cg->imports.num_memory) {
            wasm_emit_null_terminated_string(ba, node->liter->str_val, str_length);
        } else {
            wasm_emit_chars(ba, node->liter->str_val, str_length);
        }
    }
}

void wasm_emit_module(struct cg_wasm *cg, struct ast_node *node)
{
    assert(node->node_type == BLOCK_NODE);
    struct byte_array section;
    struct byte_array *ba = &cg->ba;    
    ba_init(&section, 17);
    for(size_t i = 0; i < ARRAY_SIZE(wasm_magic_number); i++){
        ba_add(ba, wasm_magic_number[i]);
    }
    for(size_t i = 0; i < ARRAY_SIZE(wasm_version); i++){
        ba_add(ba, wasm_version[i]);
    }
    // 0.    custom section
    // 1.    type section       : type signature of the function
    // 2.    import section
    // 3.    function section   : associate code section with type section
    // 4.    table section
    // 5.    memory section
    // 6.    global section
    // 7.    export section
    // 8.    start section
    // 9.    element section
    // 10.   code section       : local variable info and bytecode of function
    // 11.   data section
    // 12.   data count section
    // type section
    ba_add(ba, TYPE_SECTION);       // code: 1
    _emit_type_section(cg, &section, cg->fun_types);
    _append_section(ba, &section);
    // import section
    ba_add(ba, IMPORT_SECTION);     // code: 2
    _emit_import_section(cg, &section, cg->imports.import_block);
    _append_section(ba, &section);

    // function section
    ba_add(ba, FUNCTION_SECTION);   // code: 3
    _emit_function_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // table section                // code: 4
    // memory section               // code: 5
    if(!cg->imports.num_memory){
        ba_add(ba, MEMORY_SECTION);
        _emit_memory_section(cg, &section);
        _append_section(ba, &section);
    }

    // global section               // code: 6
    if(!cg->imports.num_global){
        ba_add(ba, GLOBAL_SECTION);
        _emit_global_section(cg, &section);
        _append_section(ba, &section);
    }
    // export section               // code: 7
    ba_add(ba, EXPORT_SECTION); 
    _emit_export_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // start section                // code: 8
    // element section              // code: 9

    // data count section           // code: 12, data count must before code section
    if (array_size(&cg->data_block->block->nodes)) {
        ba_add(ba, DATA_COUNT_SECTION); 
        wasm_emit_uint(ba, 1); //   data count size
        wasm_emit_uint(ba, 1); //   data count
    }
    // code section                 // code: 10
    ba_add(ba, CODE_SECTION); 
    _emit_code_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // data section                 // code: 11
    if(array_size(&cg->data_block->block->nodes)){
        ba_add(ba, DATA_SECTION);
        _emit_data_section(cg, &section, cg->data_block);
        _append_section(ba, &section);
    }

    // custom secion                // code: 0
    ba_deinit(&section);
}
