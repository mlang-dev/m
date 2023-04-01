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
#include "compiler/engine.h"
#include "sema/sema_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JIT {
    struct engine *engine;
    void *instance;
};

struct fun_pointer
{
    union{
        void* address;
        int (*i_fp)();
        char* (*s_fp)();
        f64 (*d_fp)();
    } fp;
};

struct JIT *jit_new(struct engine *engine);
void jit_free(struct JIT *jit);
void *jit_add_module(struct JIT *jit, void *module);
void jit_remove_module(void *resource_tracker);
typedef f64 (*target_address_double)();
typedef int (*target_address_int)();
struct fun_pointer jit_find_symbol(struct JIT *jit, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif
