/*
 * backend.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m front end initializer
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
    fn_compute_fun_info compute_fun_info;
    TargetType extend_type;
};

#ifdef __cplusplus
}
#endif

#endif
