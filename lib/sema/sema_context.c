/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include "sema/sema_context.h"
#include "clib/hash.h"
#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "sema/analyzer.h"
#include "tool/cmodule.h"
#include "util.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void enter_scope(struct sema_context *context)
{
    symboltable_push(&context->decl_2_typexps, context->scope_marker, 0);
}

void leave_scope(struct sema_context *context)
{
    symbol s;
    do {
        s = symboltable_pop(&context->decl_2_typexps);
        assert(s);
    } while (s != context->scope_marker);
}

struct sema_context *sema_context_new(struct parser *parser)
{
    struct sema_context *context = malloc(sizeof(*context));
    memset((void *)context, 0, sizeof(*context));
    array_init(&context->nongens, sizeof(struct type_exp *));
    array_init(&context->used_builtin_names, sizeof(symbol));
    context->parser = parser;
    symboltable_init(&context->typename_2_typexps);
    symboltable_init(&context->decl_2_typexps);
    symboltable_init(&context->varname_2_asts);
    hashtable_init(&context->ext_typename_2_asts);
    hashtable_init(&context->builtin_ast);
    hashtable_init(&context->generic_ast);
    hashtable_init(&context->specialized_ast);
    hashtable_init(&context->protos);
    hashtable_init(&context->calls);
    context->scope_marker = to_symbol("<enter_scope_marker>");
    struct array args;
    array_init(&args, sizeof(struct type_exp *));
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < ARRAY_SIZE(type_strings); i++) {
        symbol type_name = to_symbol(type_strings[i]);
        struct type_exp *exp = (struct type_exp *)create_type_oper(type_name, i, &args);
        push_symbol_type(&context->typename_2_typexps, type_name, exp);
    }
    char libpath[PATH_MAX];
    char *mpath = get_exec_path();
    join_path(libpath, mpath, "/mlib/stdio.m");
    struct block_node *block = parse_file(context->parser, libpath);
    struct array builtins;
    array_init(&builtins, sizeof(struct exp_node *));
    for (size_t i = 0; i < array_size(&block->nodes); i++) {
        struct exp_node *node = *(struct exp_node **)array_get(&block->nodes, i);
        array_push(&builtins, &node);
    }
    join_path(libpath, mpath, "/mlib/math.m");
    block = parse_file(context->parser, libpath);
    array_add(&builtins, &block->nodes);
    for (size_t i = 0; i < array_size(&builtins); i++) {
        struct exp_node *node = *(struct exp_node **)array_get(&builtins, i);
        assert(node->node_type == PROTOTYPE_NODE);
        struct prototype_node *proto = (struct prototype_node *)node;
        analyze(context, node);
        push_symbol_type(&context->decl_2_typexps, proto->name, proto->base.type);
        hashtable_set_p(&context->builtin_ast, proto->name, node);
        //string type = to_string(proto->base.type);
    }
    return context;
}

void sema_context_free(struct sema_context *context)
{
    hashtable_deinit(&context->ext_typename_2_asts);
    hashtable_deinit(&context->specialized_ast);
    hashtable_deinit(&context->generic_ast);
    hashtable_deinit(&context->builtin_ast);
    hashtable_deinit(&context->protos);
    hashtable_deinit(&context->calls);
    symboltable_deinit(&context->varname_2_asts);
    symboltable_deinit(&context->decl_2_typexps);
    symboltable_deinit(&context->typename_2_typexps);
    array_deinit(&context->used_builtin_names);
    array_deinit(&context->nongens);
    free(context);
}
