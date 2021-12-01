/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for clang wasm builtin functions
 */
#ifndef __MLANG_WASM_H__
#define __MLANG_WASM_H__
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))


typedef int32_t i32;
#define OPCODE_I32ADD   0x6A
#define OPCODE_I32SUB   0x6B
#define OPCODE_I32MUL   0x6C
#define OPCODE_I32DIV_S 0x6D

size_t get_mem_size();
int grow_mem(size_t size);

#ifdef __cplusplus
}
#endif

#endif
