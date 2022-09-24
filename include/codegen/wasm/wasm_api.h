/*
 * wasm_abi.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to compute wasm function info
 */
#ifndef __WASM_API_H__
#define __WASM_API_H__

#include "clib/byte_array.h"
#include "clib/typedef.h"
#include "clib/string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct byte_array* WasmModule;
typedef u8 Instruction;

void wasm_emit_instruction(WasmModule module, Instruction ins);
u8 wasm_get_emit_size(u64 value);
u8 wasm_emit_uint(WasmModule module, u64 value);
u8 wasm_emit_int(WasmModule module, i64 value);

u8 wasm_emit_f32(WasmModule module, f32 value);
u8 wasm_emit_f64(WasmModule module, f64 value);
void wasm_emit_const_i32(WasmModule module, i32 const_value);
void wasm_emit_const_f64(WasmModule module, double const_value);
void wasm_emit_const_f32(WasmModule module, float const_value);

void wasm_emit_chars(WasmModule module, const char *str, u32 len);
void wasm_emit_null_terminated_string(WasmModule module, const char *str, u32 len);
void wasm_emit_string(WasmModule module, string *str);

#ifdef __cplusplus
}
#endif

#endif //__WASM_API_H__
