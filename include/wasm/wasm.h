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

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))


typedef signed int i32;

size_t get_mem_size();
int grow_mem(size_t size);

#ifdef __cplusplus
}
#endif

#endif
