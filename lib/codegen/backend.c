/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * backend-end initializer/deinitializer for create/destroy backend codegen configurations
 */
#include "clib/symbol.h"
#include "parser/ast.h"
#include "sema/type.h"
#include "sema/sema_context.h"
#include "clib/util.h"
#include "codegen/backend.h"
#include "sema/type_size_info.h"
#include <assert.h>

struct backend *g_backend = 0;

struct backend *backend_init(struct sema_context *sema_context, cg_alloc_fun cg_alloc, cg_free_fun cg_free, cg_reset_fun cg_reset)
{
    struct backend *be;
    MALLOC(be, sizeof(*be));
    be->cg = cg_alloc(sema_context);
    be->cg_free = cg_free;
    be->cg_reset = cg_reset;
    g_backend = be;
    return be;
}

void backend_deinit(struct backend *be)
{
    be->cg_free(be->cg);
    free(be);
}

void backend_reset(struct backend *be, struct sema_context *sema_context)
{
    if(be->cg_reset){
        be->cg_reset(be->cg, sema_context);
    }
}
