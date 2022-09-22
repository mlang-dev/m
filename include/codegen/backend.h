/*
 * backend.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m front end initializer
 */
#ifndef __MLANG_BACKEND_H__
#define __MLANG_BACKEND_H__

#include "sema/sema_context.h"
#include "clib/hashtable.h"
#include "codegen/fun_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*cg_alloc_fun)(struct sema_context *context);
typedef void (*cg_free_fun)(void *);

struct codegen{
    fn_compute_fun_info compute_fun_info;
    TargetType extend_type;
};

struct backend{
    struct sema_context *sema_context;
    /* 
     *  symboltable of <symbol, struct type_size_info>
     *  binding type name to type size
     */
    struct hashtable type_size_infos;

    /*
     *  custom code generator
     */
    void *cg;
    cg_free_fun cg_free;
};

struct backend *backend_init(struct sema_context *sema_context, cg_alloc_fun cg_alloc, cg_free_fun cg_free);
void backend_deinit(struct backend *be);
struct hashtable *get_type_size_infos();

#ifdef __cplusplus
}
#endif

#endif
