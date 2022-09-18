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
#include "codegen/llvm/codegen.h"
#include "codegen/env.h"
#include "sema/sema_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JIT {
    struct env *env;
    void *instance;
};

struct fun_pointer
{
    union{
        void* address;
        int (*i_fp)();
        char* (*s_fp)();
        double (*d_fp)();
    } fp;
};

struct JIT *jit_new(struct env *cg);
void jit_free(struct JIT *jit);
void add_module(struct JIT *jit, void *module);
typedef double (*target_address_double)();
typedef int (*target_address_int)();
struct fun_pointer find_target_address(struct JIT *jit, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif
