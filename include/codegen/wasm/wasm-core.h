/*
 * wasm-core.h
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * constant definitions for WebAssembly core specification
 */
#ifndef __MLANG_WASM_CORE_H__
#define __MLANG_WASM_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif


// valtype
// number type numtype
enum WasmType {
    WasmTypeVoid = 0x40,
    WasmTypeI32 = 0x7F,
    WasmTypeI64 = 0x7E,
    WasmTypeF32 = 0x7D,
    WasmTypeF64 = 0x7C,
    // vectype: vector types
    WasmTypeVector = 0x7B,
    // reftype: reference types
    WasmTypeFuncRef = 0x70,
    WasmTypeExternRef = 0x6F,
    // valtype := t:numtype => t
    //   |t:vectype => t
    //   |t:reftype => t
    // resulttype := vec(valtype)

    // function type := 0x60 rt1: resulttype rt2: resulttype
    WasmTypeFunc = 0x60
};

// limits

enum WasmLimitsType {
    WasmLimitsTypeMinOnly = 0, // 0, min
    WasmLimitsTypeMinMax // 1, min, max
};
// limits := 0x00 n:u32
//          |0x01 n:u32 m: u32

// Memory types
// memtype := lim:limits => lim

// Table types
// tabletype := et: reftype lim: limits  => lim et

// Global Types
// globaltype    := t:valuetype  m:mut   => m t
//   mut         := 0x00     => const
//               |  0x01     => var
enum WasmGlobalType{
    WasmGlobalTypeConst = 0,
    WasmGlobalTypeVar
};

// control instructions
// blocktype :=  0x40    =>  epsilon
//           |   t:valtype   =>  t
//           |   x:s33       =>  x
enum WasmInstrControl {
    WasmInstrControlEpsilon = 0x40,
    WasmInstrControlUnreachable = 0x00,
    WasmInstrControlNop = 0x01,
    WasmInstrControlBlock = 0x02,
    WasmInstrControlEnd = 0x0B,
    WasmInstrControlLoop = 0x03,
    WasmInstrControlIf = 0x04,
    WasmInstrControlElse = 0x05,
    WasmInstrControlBr = 0x0C,
    WasmInstrControlBrIf = 0x0D,
    WasmInstrControlBrTable = 0x0E,
    WasmInstrControlReturn = 0x0F,
    WasmInstrControlCall = 0x10,
    WasmInstrControlCallInd = 0x11,
};

// reference instructions
enum WasmInstrRef {
    WasmInstrRefDrop = 0x1A,
    WasmInstrRefSelect = 0x1B,
    WasmInstrRefSelectV = 0x1C,
};

// variable instructions
enum WasmInstrVar {
    WasmInstrVarLocalGet = 0x20,
    WasmInstrVarLocalSet = 0x21,
    WasmInstrVarLocalTee = 0x22,
    WasmInstrVarGlobalGet = 0x23,
    WasmInstrVarGlobalSet = 0x24,
};

// table instructions
enum WasmInstrTable {
    WasmInstrTableGet = 0x25,
    WasmInstrTableSet = 0x26,
    WasmInstrTableOp = 0xFC,
                        // 12 table.init
                        // 13 elem.drop
                        // 14 table.copy
                        // 15 table.grow
                        // 16 table.size
                        // 17 table.fill
};

// memory instructions
// memarg    :=  a:u32   o:u32   => {align a, offset o}
enum WasmInstrMem {
    WasmInstrMemI32Load = 0x28,
    WasmInstrMemI64Load = 0x29,
    WasmInstrMemF32Load = 0x2A,
    WasmInstrMemF64Load = 0x2B,
    WasmInstrMemI32Load8S = 0x2C,
    WasmInstrMemI32Load8U = 0x2D,
    WasmInstrMemI32Load16S = 0x2E,
    WasmInstrMemI32Load16U = 0x2F,
    WasmInstrMemI64Load8S = 0x30,
    WasmInstrMemI64Load8U = 0x31,
    WasmInstrMemI64Load16S = 0x32,
    WasmInstrMemI64Load16U = 0x33,
    WasmInstrMemI64Load32S = 0x34,
    WasmInstrMemI64Load32U = 0x35,

    WasmInstrMemI32Store = 0x36,
    WasmInstrMemI64Store = 0x37,
    WasmInstrMemF32Store = 0x38,
    WasmInstrMemF64Store = 0x39,

    WasmInstrMemI32Store8 = 0x3A,
    WasmInstrMemI32Store16 = 0x3B,
    WasmInstrMemI64Store8 = 0x3C,
    WasmInstrMemI64Store16 = 0x3D,
    WasmInstrMemI64Store32 = 0x3E,
    WasmInstrMemSize = 0x3F, // 00
    WasmInstrMemGrow = 0x40,
    WasmInstrMemOp = 0xFC,// 8     memory.init
                        // 9     memory data.drop
                        // 10    memory copy
                        // 11    memory fill

};

// numeric instructions
enum WasmInstrNum {
    WasmInstrNumI32Const = 0x41,
    WasmInstrNumI64Const = 0x42,
    WasmInstrNumF32Const = 0x43,
    WasmInstrNumF64Const = 0x44,
};

//TODO: move the following definitions into enum WasmInstrNum
#define OPCODE_I32EQZ 0x45
#define OPCODE_I32EQ 0x46
#define OPCODE_I32NE 0x47
#define OPCODE_I32LT_S 0x48
#define OPCODE_I32LT_U 0x49
#define OPCODE_I32GT_S 0x4A
#define OPCODE_I32GT_U 0x4B
#define OPCODE_I32LE_S 0x4C
#define OPCODE_I32LE_U 0x4D
#define OPCODE_I32GE_S 0x4E
#define OPCODE_I32GE_U 0x4F

#define OPCODE_I64EQZ 0x50
#define OPCODE_I64EQ 0x51
#define OPCODE_I64NE 0x52
#define OPCODE_I64LT_S 0x53
#define OPCODE_I64LT_U 0x54
#define OPCODE_I64GT_S 0x55
#define OPCODE_I64GT_U 0x56
#define OPCODE_I64LE_S 0x57
#define OPCODE_I64LE_U 0x58
#define OPCODE_I64GE_S 0x59
#define OPCODE_I64GE_U 0x5A

#define OPCODE_F32EQ 0x5B
#define OPCODE_F32NE 0x5C
#define OPCODE_F32LT 0x5D
#define OPCODE_F32GT 0x5E
#define OPCODE_F32LE 0x5F
#define OPCODE_F32GE 0x60

#define OPCODE_F64EQ 0x61
#define OPCODE_F64NE 0x62
#define OPCODE_F64LT 0x63
#define OPCODE_F64GT 0x64
#define OPCODE_F64LE 0x65
#define OPCODE_F64GE 0x66

#define OPCODE_I32CLZ 0x67
#define OPCODE_I32CTZ 0x68
#define OPCODE_I32POPCNT 0x69
#define OPCODE_I32ADD 0x6A
#define OPCODE_I32SUB 0x6B
#define OPCODE_I32MUL 0x6C
#define OPCODE_I32DIV_S 0x6D
#define OPCODE_I32DIV_U 0x6E
#define OPCODE_I32REM_S 0x6F
#define OPCODE_I32REM_U 0x70

#define OPCODE_I32AND 0x71
#define OPCODE_I32OR 0x72
#define OPCODE_I32XOR 0x73
#define OPCODE_I32SHL 0x74
#define OPCODE_I32SHR_S 0x75
#define OPCODE_I32SHR_U 0x76
#define OPCODE_I32ROTL 0x77
#define OPCODE_I32ROTR 0x78

#define OPCODE_I64CLZ 0x79
#define OPCODE_I64CTZ 0x7A
#define OPCODE_I64POPCNT 0x7B
#define OPCODE_I64ADD 0x7C
#define OPCODE_I64SUB 0x7D
#define OPCODE_I64MUL 0x7E
#define OPCODE_I64DIV_S 0x7F
#define OPCODE_I64DIV_U 0x80
#define OPCODE_I64REM_S 0x81
#define OPCODE_I64REM_U 0x82

#define OPCODE_I64AND 0x83
#define OPCODE_I64OR 0x84
#define OPCODE_I64XOR 0x85
#define OPCODE_I64SHL 0x86
#define OPCODE_I64SHR_S 0x87
#define OPCODE_I64SHR_U 0x88
#define OPCODE_I64ROTL 0x89
#define OPCODE_I64ROTR 0x8A

#define OPCODE_F32ABS 0x8B
#define OPCODE_F32NEG 0x8C
#define OPCODE_F32CEIL 0x8D
#define OPCODE_F32FLOOR 0x8E
#define OPCODE_F32TRUNC 0x8F
#define OPCODE_F32NEAREST 0x90
#define OPCODE_F32SQRT 0x91
#define OPCODE_F32ADD 0x92
#define OPCODE_F32SUB 0x93
#define OPCODE_F32MUL 0x94
#define OPCODE_F32DIV 0x95
#define OPCODE_F32MIN 0x96
#define OPCODE_F32MAX 0x97
#define OPCODE_F32COPYSIGN 0x98

#define OPCODE_F64ABS 0x99
#define OPCODE_F64NEG 0x9A
#define OPCODE_F64CEIL 0x9B
#define OPCODE_F64FLOOR 0x9C
#define OPCODE_F64TRUNC 0x9D
#define OPCODE_F64NEAREST 0x9E
#define OPCODE_F64SQRT 0x9F
#define OPCODE_F64ADD 0xA0
#define OPCODE_F64SUB 0xA1
#define OPCODE_F64MUL 0xA2
#define OPCODE_F64DIV 0xA3
#define OPCODE_F64MIN 0xA4
#define OPCODE_F64MAX 0xA5
#define OPCODE_F64COPYSIGN 0xA6

#define OPCODE_I32WRAPI64 0xA7
#define OPCODE_I32TRUNC_F32S 0xA8
#define OPCODE_I32TRUNC_F32U 0xA9
#define OPCODE_I32TRUNC_F64S 0xAA
#define OPCODE_I32TRUNC_F64U 0xAB
#define OPCODE_I64EXTEND_I32S 0xAC
#define OPCODE_I64EXTEND_I32U 0xAD
#define OPCODE_I64TRUNC_F32S 0xAE
#define OPCODE_I64TRUNC_F32U 0xAF
#define OPCODE_I64TRUNC_F64S 0xB0
#define OPCODE_I64TRUNC_F64U 0xB1

#define OPCODE_F32CONVERT_I32S 0xB2
#define OPCODE_F32CONVERT_I32U 0xB3
#define OPCODE_F32CONVERT_I64S 0xB4
#define OPCODE_F32CONVERT_I64U 0xB5
#define OPCODE_F32DEMOTE_F64 0xB6
#define OPCODE_F64CONVERT_I32S 0xB7
#define OPCODE_F64CONVERT_I32U 0xB8
#define OPCODE_F64CONVERT_I64S 0xB9
#define OPCODE_F64CONVERT_I64U 0xBA
#define OPCODE_F64PROMOTE_F32 0xBB
#define OPCODE_I32REINTERPRET_F32 0xBC
#define OPCODE_I64REINTERPRET_F64 0xBD
#define OPCODE_F32REINTERPRET_I32 0xBE
#define OPCODE_F64REINTERPRET_I64 0xBF

#define OPCODE_I32EXTEND8_S 0xC0
#define OPCODE_I32EXTEND16_S 0xC1
#define OPCODE_I64EXTEND8_S 0xC2
#define OPCODE_I64EXTEND16_S 0xC3
#define OPCODE_I64EXTEND32_S 0xC4

#define OPCODE_TRUNC_SAT 0xFC // 0 i32.trunc_sat_f32_s
                              // 1 i32.trunc_sat_f32_u
                              // 2 i32.trunc_sat_f64_s
                              // 3 i32.trunc_sat_f64_u
                              // 4 i64.trunc_sat_f32_s
                              // 5 i64.trunc_sat_f32_u
                              // 6 i64.trunc_sat_f64_s
                              // 7 i64.trunc_sat_f64_u

// vector instructions
enum WasmSection {
    WasmSectionCustom = 0,
    WasmSectionType,
    WasmSectionImport,
    WasmSectionFunction,
    WasmSectionTable,
    WasmSectionMemory,
    WasmSectionGlobal,
    WasmSectionExport,
    WasmSectionStart,
    WasmSectionElement,
    WasmSectionCode,
    WasmSectionData,
    WasmSectionDataCount
};

enum WasmExportType {
    WasmExportTypeFunc = 0,
    WasmExportTypeTable,
    WasmExportTypeMemory,
    WasmExportTypeGlobal
};

enum WasmImportType {
    WasmImportTypeFunc = 0,
    WasmImportTypeTable,
    WasmImportTypeMemory,
    WasmImportTypeGlobal
};

enum WasmDataSegmentType {
    WasmDataSegmentTypeActive = 0,  //active, {memory 0, offset e}
    WasmDataSegmentTypePassive,     //passive
    WasmDataSegmentTypeActiveM     //active, {memory idx, offset e}
};

#ifdef __cplusplus
}
#endif

#endif //__MLANG_WASM_CORE_H__
