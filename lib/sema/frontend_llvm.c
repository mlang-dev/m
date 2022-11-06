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

struct frontend *frontend_llvm_init(const char *stdio_filepath, const char *math_filepath, bool is_repl)
{
    error_init();
    symbols_init();
    types_init();
    terminal_init();
    ast_init();
    struct frontend*fe;
    MALLOC(fe, sizeof(*fe));
    fe->parser = parser_new();
    struct ast_node *stdio = 0;
    struct ast_node *math = 0;
    char libpath[4096];
    char *mpath = get_exec_path();
    join_path(libpath, sizeof(libpath), mpath, stdio_filepath);
    stdio = parse_file(fe->parser, libpath);
    join_path(libpath, sizeof(libpath), mpath, math_filepath);
    math = parse_file(fe->parser, libpath);
    fe->sema_context = sema_context_new(&fe->parser->symbol_2_int_types, stdio, math, is_repl);
    return fe;
}
