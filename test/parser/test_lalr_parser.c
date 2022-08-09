/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "parser/lalr_parser.h"
#include "parser/ast.h"
#include "tutil.h"
#include "test.h"
#include "lexer/init.h"
#include <stdio.h>

TEST(test_lalr_parser, var_decl)
{
    frontend_init();
    frontend_deinit();
}

int test_lr_parser()
{
    UNITY_BEGIN();
    RUN_TEST(test_lalr_parser_var_decl);
    return UNITY_END();
}
