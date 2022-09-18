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

struct cg *gcg = 0;

struct cg *backend_init(struct sema_context *sema_context)
{
    struct cg *cg;
    MALLOC(cg, sizeof(*cg));
    cg->sema_context = sema_context;
    hashtable_init_with_value_size(&cg->type_size_infos, sizeof(struct type_size_info), 0);
    gcg = cg;
    return cg;
}

void backend_deinit(struct cg *cg)
{
    hashtable_deinit(&cg->type_size_infos);
    free(cg);
}

struct hashtable *get_type_size_infos()
{
    assert(gcg);
    return &gcg->type_size_infos;
}
