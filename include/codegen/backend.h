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


#ifdef __cplusplus
extern "C" {
#endif

struct cg{
    struct sema_context *sema_context;
    /* 
     *  symboltable of <symbol, struct type_size_info>
     *  binding type name to type size
     */
    struct hashtable type_size_infos;
};

struct cg *backend_init(struct sema_context *sema_context);
void backend_deinit(struct cg *cg);
struct hashtable *get_type_size_infos();

#ifdef __cplusplus
}
#endif

#endif
