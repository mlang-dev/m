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
#include "codegen/type_size_info.h"
#include <assert.h>

struct backend *g_backend = 0;

struct backend *backend_init(struct sema_context *sema_context, cg_alloc_fun cg_alloc, cg_free_fun cg_free)
{
    struct backend *be;
    MALLOC(be, sizeof(*be));
    be->cg = cg_alloc(sema_context);
    be->cg_free = cg_free;
    g_backend = be;
    tsi_init();
    return be;
}

void backend_deinit(struct backend *be)
{
    tsi_deinit();
    be->cg_free(be->cg);
    free(be);
}
