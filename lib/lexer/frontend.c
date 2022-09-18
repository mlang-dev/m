/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * front-end initializer/deinitializer for create/destroy front-end configurations
 */
#include "clib/symbol.h"
#include "lexer/token.h"
#include "parser/ast.h"
#include "sema/type.h"

void frontend_init()
{
    symbols_init();
    types_init();
    token_init();
    ast_init();
}

void frontend_deinit()
{
    ast_deinit();
    token_deinit();
    types_deinit();
    symbols_deinit();
}
