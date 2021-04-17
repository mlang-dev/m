/*
 * codegen.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for LLVM IR codegen
 */
#ifndef __COMPUTE_FUN_INFO_H__
#define __COMPUTE_FUN_INFO_H__

#include "codegen/fun_info.h"
#include "codegen/target_info.h"

#ifdef __cplusplus
extern "C" {
#endif

void winx86_64_compute_fun_info(struct fun_info *fi);
void x86_64_compute_fun_info(struct fun_info *fi);
void compute_fun_info(struct fun_info *fi);

#ifdef __cplusplus
}
#endif

#endif //__COMPUTE_FUN_INFO_H__
