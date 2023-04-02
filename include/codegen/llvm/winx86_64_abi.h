/*
 * winx86_64_compute_fun_info.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to compute winx86/64 function info
 */
#ifndef __WINX86_64_COMPUTE_FUN_INFO_H__
#define __WINX86_64_COMPUTE_FUN_INFO_H__

#include "codegen/fun_info.h"
#include "codegen/codegen.h"

#ifdef __cplusplus
extern "C" {
#endif

void winx86_64_compute_fun_info(struct codegen *cg, struct fun_info *fi);


#ifdef __cplusplus
}
#endif

#endif //__WINX86_64_COMPUTE_FUN_INFO_H__
