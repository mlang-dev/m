/*
 * wasm_abi.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to compute wasm function info
 */
#ifndef __WASM_ABI_H__
#define __WASM_ABI_H__

#include "codegen/fun_info.h"
#include "codegen/codegen.h"

#ifdef __cplusplus
extern "C" {
#endif

void wasm_compute_fun_info(struct codegen *ti, struct fun_info *fi);


#ifdef __cplusplus
}
#endif

#endif //__WASM_ABI_H__
