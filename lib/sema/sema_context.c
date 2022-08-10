/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include "sema/sema_context.h"
#include "clib/hash.h"
#include "clib/hashset.h"
#include "clib/hashtable.h"
#include "clib/util.h"
#include "sema/analyzer.h"
#include "tool/cmodule.h"
#include <assert.h>
#include <limits.h>

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

struct sema_context *sema_context_new(struct hashtable *symbol_2_int_types, struct ast_node *stdio, struct ast_node *math, bool is_repl)
{
    struct sema_context *context;
    CALLOC(context, 1, sizeof(*context));
    context->symbol_2_int_types = symbol_2_int_types;
    context->is_repl = is_repl;
    array_init(&context->nongens, sizeof(struct type_exp *));
    array_init(&context->used_builtin_names, sizeof(symbol));
    symboltable_init(&context->typename_2_typexps);
    symboltable_init(&context->decl_2_typexps);
    symboltable_init(&context->varname_2_asts);
    stack_init(&context->func_stack, sizeof(struct ast_node *));
    hashtable_init(&context->ext_typename_2_asts);
    hashtable_init(&context->builtin_ast);
    hashtable_init(&context->generic_ast);
    hashtable_init(&context->specialized_ast);
    array_init(&context->new_specialized_asts, sizeof(struct ast_node *));
    hashtable_init(&context->func_types);
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

    struct array builtins;
    array_init(&builtins, sizeof(struct ast_node *));
    if (stdio){
        for (size_t i = 0; i < array_size(&stdio->block->nodes); i++) {
            struct ast_node *node = *(struct ast_node **)array_get(&stdio->block->nodes, i);
            array_push(&builtins, &node);
        }
    }
    if (math){
        array_add(&builtins, &math->block->nodes);
    }
    for (size_t i = 0; i < array_size(&builtins); i++) {
        struct ast_node *node = *(struct ast_node **)array_get(&builtins, i);
        assert(node->node_type == FUNC_TYPE_NODE);
        analyze(context, node);
        push_symbol_type(&context->decl_2_typexps, node->ft->name, node->type);
        hashtable_set_p(&context->builtin_ast, node->ft->name, node);
        //string type = to_string(func_type->base.type);
    }
    return context;
}

void sema_context_free(struct sema_context *context)
{
    hashtable_deinit(&context->ext_typename_2_asts);
    hashtable_deinit(&context->specialized_ast);
    hashtable_deinit(&context->generic_ast);
    hashtable_deinit(&context->builtin_ast);
    hashtable_deinit(&context->func_types);
    hashtable_deinit(&context->calls);
    stack_deinit(&context->func_stack);
    symboltable_deinit(&context->varname_2_asts);
    symboltable_deinit(&context->decl_2_typexps);
    symboltable_deinit(&context->typename_2_typexps);
    array_deinit(&context->used_builtin_names);
    array_deinit(&context->nongens);
    array_deinit(&context->new_specialized_asts);
    FREE(context);
}

struct ast_node *find_generic_fun(struct sema_context *context, symbol fun_name)
{
    return hashtable_get(&context->generic_ast, string_get(fun_name));
}
