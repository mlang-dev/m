/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * backend-end initializer/deinitializer for create/destroy backend codegen configurations
 */
#include "clib/symbol.h"
#include "lexer/token.h"
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
    hashtable_init_with_value_size(&be->type_size_infos, sizeof(struct type_size_info), 0);
    g_backend = be;
    return be;
}

void backend_deinit(struct backend *be)
{
    hashtable_deinit(&be->type_size_infos);
    be->cg_free(be->cg);
    free(be);
}

struct hashtable *get_type_size_infos()
{
    assert(g_backend);
    return &g_backend->type_size_infos;
}
