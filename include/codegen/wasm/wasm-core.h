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

    WasmInstrNumI32EQZ = 0x45,
    WasmInstrNumI32EQ = 0x46,
    WasmInstrNumI32NE = 0x47,
    WasmInstrNumI32LTS = 0x48,
    WasmInstrNumI32LTU = 0x49,
    WasmInstrNumI32GTS = 0x4A,
    WasmInstrNumI32GTU = 0x4B,
    WasmInstrNumI32LES = 0x4C,
    WasmInstrNumI32LEU = 0x4D,
    WasmInstrNumI32GES = 0x4E,
    WasmInstrNumI32GEU = 0x4F,

    WasmInstrNumI64EQZ = 0x50,
    WasmInstrNumI64EQ = 0x51,
    WasmInstrNumI64NE = 0x52,

    WasmInstrNumI64LTS = 0x53,
    WasmInstrNumI64LTU = 0x54,
    WasmInstrNumI64GTS = 0x55,
    WasmInstrNumI64GTU = 0x56,
    WasmInstrNumI64LES = 0x57,
    WasmInstrNumI64LEU = 0x58,
    WasmInstrNumI64GES = 0x59,
    WasmInstrNumI64GEU = 0x5A,

    WasmInstrNumF32EQ = 0x5B,
    WasmInstrNumF32NE = 0x5C,
    WasmInstrNumF32LT = 0x5D,
    WasmInstrNumF32GT = 0x5E,
    WasmInstrNumF32LE = 0x5F,
    WasmInstrNumF32GE = 0x60,

    WasmInstrNumF64EQ = 0x61,
    WasmInstrNumF64NE = 0x62,
    WasmInstrNumF64LT = 0x63,
    WasmInstrNumF64GT = 0x64,
    WasmInstrNumF64LE = 0x65,
    WasmInstrNumF64GE = 0x66,

    WasmInstrNumI32CLZ = 0x67,
    WasmInstrNumI32CTZ = 0x68,
    WasmInstrNumI32POPCNT = 0x69,
    WasmInstrNumI32ADD = 0x6A,
    WasmInstrNumI32SUB = 0x6B,
    WasmInstrNumI32MUL = 0x6C,
    WasmInstrNumI32DIVS = 0x6D,
    WasmInstrNumI32DIVU = 0x6E,
    WasmInstrNumI32REMS = 0x6F,
    WasmInstrNumI32REMU = 0x70,
    WasmInstrNumI32AND = 0x71,
    WasmInstrNumI32OR = 0x72,
    WasmInstrNumI32XOR = 0x73,
    WasmInstrNumI32SHL = 0x74,
    WasmInstrNumI32SHRS = 0x75,
    WasmInstrNumI32SHRU = 0x76,
    WasmInstrNumI32ROTL = 0x77,
    WasmInstrNumI32ROTR = 0x78,
    WasmInstrNumI64CLZ = 0x79,
    WasmInstrNumI64CTZ = 0x7A,
    WasmInstrNumI64POPCNT = 0x7B,
    WasmInstrNumI64ADD = 0x7C,
    WasmInstrNumI64SUB = 0x7D,
    WasmInstrNumI64MUL = 0x7E,
    WasmInstrNumI64DIVS = 0x7F,
    WasmInstrNumI64DIVU = 0x80,
    WasmInstrNumI64REMS = 0x81,
    WasmInstrNumI64REMU = 0x82,
    WasmInstrNumI64AND = 0x83,
    WasmInstrNumI64OR = 0x84,
    WasmInstrNumI64XOR = 0x85,
    WasmInstrNumI64SHL = 0x86,
    WasmInstrNumI64SHRS = 0x87,
    WasmInstrNumI64SHRU = 0x88,
    WasmInstrNumI64ROTL = 0x89,
    WasmInstrNumI64ROTR = 0x8A,

    WasmInstrNumF32ABS = 0x8B,
    WasmInstrNumF32NEG = 0x8C,
    WasmInstrNumF32CEIL = 0x8D,
    WasmInstrNumF32FLOOR = 0x8E,
    WasmInstrNumF32TRUNC = 0x8F,
    WasmInstrNumF32NEAREST = 0x90,
    WasmInstrNumF32SQRT = 0x91,
    WasmInstrNumF32ADD = 0x92,
    WasmInstrNumF32SUB = 0x93,
    WasmInstrNumF32MUL = 0x94,
    WasmInstrNumF32DIV = 0x95,
    WasmInstrNumF32MIN = 0x96,
    WasmInstrNumF32MAX = 0x97,
    WasmInstrNumF32COPYSIGN = 0x98,
    WasmInstrNumF64ABS = 0x99,
    WasmInstrNumF64NEG = 0x9A,
    WasmInstrNumF64CEIL = 0x9B,
    WasmInstrNumF64FLOOR = 0x9C,
    WasmInstrNumF64TRUNC = 0x9D,
    WasmInstrNumF64NEAREST = 0x9E,
    WasmInstrNumF64SQRT = 0x9F,
    WasmInstrNumF64ADD = 0xA0,
    WasmInstrNumF64SUB = 0xA1,
    WasmInstrNumF64MUL = 0xA2,
    WasmInstrNumF64DIV = 0xA3,
    WasmInstrNumF64MIN = 0xA4,
    WasmInstrNumF64MAX = 0xA5,
    WasmInstrNumF64COPYSIGN = 0xA6,
    WasmInstrNumI32WRAPI64 = 0xA7,
    WasmInstrNumI32TRUNCF32S = 0xA8,
    WasmInstrNumI32TRUNCF32U = 0xA9,
    WasmInstrNumI32TRUNCF64S = 0xAA,
    WasmInstrNumI32TRUNCF64U = 0xAB,
    WasmInstrNumI64EXTENDI32S = 0xAC,
    WasmInstrNumI64EXTENDI32U = 0xAD,
    WasmInstrNumI64TRUNCF32S = 0xAE,
    WasmInstrNumI64TRUNCF32U = 0xAF,
    WasmInstrNumI64TRUNCF64S = 0xB0,
    WasmInstrNumI64TRUNCF64U = 0xB1,
    WasmInstrNumF32CONVERTI32S = 0xB2,
    WasmInstrNumF32CONVERTI32U = 0xB3,
    WasmInstrNumF32CONVERTI64S = 0xB4,
    WasmInstrNumF32CONVERTI64U = 0xB5,
    WasmInstrNumF32DEMOTEF64 = 0xB6,
    WasmInstrNumF64CONVERTI32S = 0xB7,
    WasmInstrNumF64CONVERTI32U = 0xB8,
    WasmInstrNumF64CONVERTI64S = 0xB9,
    WasmInstrNumF64CONVERTI64U = 0xBA,
    WasmInstrNumF64PROMOTEF32 = 0xBB,
    WasmInstrNumI32REINTERPRETF32 = 0xBC,
    WasmInstrNumI64REINTERPRETF64 = 0xBD,
    WasmInstrNumF32REINTERPRETI32 = 0xBE,
    WasmInstrNumF64REINTERPRETI64 = 0xBF,
    WasmInstrNumI32EXTEND8S = 0xC0,
    WasmInstrNumI32EXTEND16S = 0xC1,
    WasmInstrNumI64EXTEND8S = 0xC2,
    WasmInstrNumI64EXTEND16S = 0xC3,
    WasmInstrNumI64EXTEND32S = 0xC4,
    WasmInstrNumTRUNCSAT = 0xFC, // 0 i32.trunc_sat_f32_s
                              // 1 i32.trunc_sat_f32_u
                              // 2 i32.trunc_sat_f64_s
                              // 3 i32.trunc_sat_f64_u
                              // 4 i64.trunc_sat_f32_s
                              // 5 i64.trunc_sat_f32_u
                              // 6 i64.trunc_sat_f64_s
                              // 7 i64.trunc_sat_f64_u
};

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
