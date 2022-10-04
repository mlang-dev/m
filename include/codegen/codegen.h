/*
 * codegen.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for common codegen data structures
 */
#ifndef __MLANG_CODEGEN_H__
#define __MLANG_CODEGEN_H__

#include "sema/sema_context.h"
#include "clib/hashtable.h"
#include "codegen/fun_info.h"

#ifdef __cplusplus
extern "C" {
#endif

struct codegen{
    /// target info
    struct target_info *target_info;

    fn_compute_fun_info compute_fun_info;

    struct sema_context *sema_context;
};

typedef struct codegen* (*cg_alloc_fun)(struct sema_context *context);
typedef void (*cg_free_fun)(struct codegen*);

#ifdef __cplusplus
}
#endif

#endif
