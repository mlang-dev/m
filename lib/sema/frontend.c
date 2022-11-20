/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * front-end initializer/deinitializer for create/destroy front-end configurations
 */
#include "clib/symbol.h"
#include "parser/ast.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include "clib/util.h"
#include "error/error.h"

struct frontend *frontend_init()
{
    symbols_init();
    error_init();
    types_init();
    terminal_init();
    ast_init();
    struct frontend*fe;
    MALLOC(fe, sizeof(*fe));
    fe->parser = parser_new();
    fe->sema_context = sema_context_new(&fe->parser->symbol_2_int_types, 0, 0, false);
    return fe;
}

void frontend_deinit(struct frontend *fe)
{
    ast_deinit();
    terminal_deinit();
    types_deinit();
    sema_context_free(fe->sema_context);
    parser_free(fe->parser);
    free(fe);
    error_deinit();
    symbols_deinit();
}
