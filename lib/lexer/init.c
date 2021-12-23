/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * front-end initializer/deinitializer for create/destroy front-end configurations
 */
#include "clib/symbol.h"
#include "lexer/token.h"

void frontend_init()
{
    symbols_init();
    token_init();
}

void frontend_deinit()
{
    token_deinit();
    symbols_deinit();
}
