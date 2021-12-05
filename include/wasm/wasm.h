/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for wasm data structures
 */
#ifndef __MLANG_WASM_H__
#define __MLANG_WASM_H__
#include <stddef.h>
#include <stdint.h>
#include "clib/array.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WASM
#define EXPORT __attribute__((visibility("default")))
#else 
#define EXPORT 
#endif 

typedef int32_t i32;
#define OPCODE_I32ADD   0x6A
#define OPCODE_I32SUB   0x6B
#define OPCODE_I32MUL   0x6C
#define OPCODE_I32DIV_S 0x6D

struct wasm_module{
    struct array types;   //1: type section
    struct array functions; //3: function section
    struct array exports; //7: export section
    struct array codes;   //10: code section
};

struct wasm_module *wasm_module_new();
void wasm_module_free(struct wasm_module *wm);

#ifdef __cplusplus
}
#endif

#endif
