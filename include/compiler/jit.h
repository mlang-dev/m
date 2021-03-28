/*
 * jit.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for jit.c
 */

#ifndef __MLANG_JIT_H__
#define __MLANG_JIT_H__

#include "clib/util.h"
#include "codegen/codegen.h"
#include "sema/env.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JIT {
    struct code_generator* cg;
    void* instance;
    struct env* env;
};

struct JIT* jit_new(struct code_generator* cg);
void jit_free(struct JIT* jit);
void add_module(struct JIT* jit, void* module);
typedef double (*target_address_double)();
typedef int (*target_address_int)();
void* find_target_address(struct JIT* jit, const char* symbol);

#ifdef __cplusplus
}
#endif

#endif
