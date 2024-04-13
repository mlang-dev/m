/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * front-end initializer/deinitializer for create/destroy front-end configurations
 * the frontend is initialized with system path and is_repl flag
 * the system path is the path to the system files: /src/lib/math.m...
 */
#include "clib/symbol.h"
#include "parser/ast.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include "clib/util.h"
#include "app/error.h"
#include <dirent.h>


struct array _get_file_paths(const char *sys_path)
{
    ARRAY_STRING(file_paths);
    struct dirent *dp;
    DIR *dfd;
    if ((dfd = opendir(sys_path)) == NULL)
    {
        fprintf(stderr, "Can't open %s\n", sys_path);
        return file_paths;
    }
    string file_path;
    string_init(&file_path);
    while ((dp = readdir(dfd)) != NULL)    
    {
        if (dp->d_name[0] == '.')
            continue;
        string_copy_chars(&file_path, sys_path);
        string_add_chars(&file_path, "/");
        string_add_chars(&file_path, dp->d_name);
        array_push(&file_paths, &file_path);
        string_init(&file_path);
    }
    closedir(dfd);
    return file_paths;
}

struct frontend *frontend_sys_init(const char *sys_path, bool is_repl)
{
    struct frontend*fe;
    MALLOC(fe, sizeof(*fe));
    fe->parser = parser_new();
    struct ast_node *sys_block = block_node_new_empty();
    struct array file_paths = _get_file_paths(sys_path);
    for (size_t i = 0; i < array_size(&file_paths); i++)
    {
        string *file_path = array_get(&file_paths, i);
        struct ast_node *sys = parse_file(fe->parser, string_get(file_path));
        block_node_add_block(sys_block, sys);
        free_block_node(sys, false);
    }
    fe->sema_context = sema_context_new(fe->parser->tc, sys_block, is_repl);
    free_block_node(sys_block, false);
    array_deinit(&file_paths);
    return fe;
}
