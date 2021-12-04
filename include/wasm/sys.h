/*
 * sys.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for clang wasm sys builtin functions
 */

#ifndef __MLANG_SYS_H__
#define __MLANG_SYS_H__
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
size_t get_mem_size();
int grow_mem(size_t size);


#ifdef __cplusplus
}
#endif

#endif
