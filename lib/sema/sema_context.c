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
#include "clib/array.h"
#include "sema/analyzer.h"
#include "codegen/type_size_info.h"
#include "tool/cmodule.h"
#include <assert.h>
#include <limits.h>

size_t enter_scope(struct sema_context *context)
{
    symboltable_push(&context->decl_2_typexps, context->scope_marker, 0);
    return ++context->scope_level;
}

size_t leave_scope(struct sema_context *context)
{
    symbol s;
    do {
        s = symboltable_pop(&context->decl_2_typexps);
        assert(s);
    } while (s != context->scope_marker);
    return --context->scope_level;
}

struct sema_context *sema_context_new(struct hashtable *symbol_2_int_types, struct ast_node *stdio, struct ast_node *math, bool is_repl)
{
    struct sema_context *context;
    CALLOC(context, 1, sizeof(*context));
    context->symbol_2_int_types = symbol_2_int_types;
    context->is_repl = is_repl;
    array_init(&context->nongens, sizeof(struct type_expr *));
    array_init(&context->used_builtin_names, sizeof(symbol));
    symboltable_init(&context->typename_2_typexps);
    symboltable_init(&context->decl_2_typexps);
    symboltable_init(&context->varname_2_asts);
    hashtable_init(&context->gvar_name_2_ast);
    stack_init(&context->func_stack, sizeof(struct ast_node *));
    hashtable_init(&context->struct_typename_2_asts);
    hashtable_init(&context->builtin_ast);
    hashtable_init(&context->generic_ast);
    hashtable_init(&context->specialized_ast);
    array_init(&context->new_specialized_asts, sizeof(struct ast_node *));
    hashtable_init(&context->func_types);
    hashtable_init(&context->calls);
    context->scope_level = 0;
    context->scope_marker = to_symbol("<enter_scope_marker>");
    struct array args;
    array_init(&args, sizeof(struct type_expr *));
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < ARRAY_SIZE(type_symbols); i++) {
        symbol type_name = type_symbols[i];
        struct type_expr *exp = (struct type_expr *)create_type_oper(type_name, i, &args);
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
    hashtable_deinit(&context->struct_typename_2_asts);
    hashtable_deinit(&context->specialized_ast);
    hashtable_deinit(&context->generic_ast);
    hashtable_deinit(&context->builtin_ast);
    hashtable_deinit(&context->func_types);
    hashtable_deinit(&context->calls);
    stack_deinit(&context->func_stack);
    hashtable_deinit(&context->gvar_name_2_ast);
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

struct field_info sc_get_field_info(struct sema_context *sc, symbol struct_name, symbol field_name)
{
    struct ast_node *struct_node = hashtable_get_p(&sc->struct_typename_2_asts, struct_name);
    assert(struct_node->type->kind == KIND_OPER);
    struct type_oper *struct_type = (struct type_oper*)struct_node->type;
    struct field_info field;
    int index = find_member_index(struct_node, field_name);
    struct type_expr *field_type = *(struct type_expr **)array_get(&struct_type->args, index);
    struct struct_layout *sl = get_type_size_info(struct_node->type).sl;
    field.offset = *(u64 *)array_get(&sl->field_offsets, index) / 8;
    field.align = get_type_align(field_type) / 8;
    field.type = field_type;
    return field;
}

struct field_info sc_get_field_info_from_root(struct sema_context *sc, struct ast_node* index)
{
    assert(index->node_type == MEMBER_INDEX_NODE);
    struct ast_node *root = index;
    struct array fields;
    struct array types;
    array_init(&fields, sizeof(symbol));
    array_init(&types, sizeof(symbol));
    while(root->node_type == MEMBER_INDEX_NODE){
        assert(root->index->index->node_type == IDENT_NODE);
        array_push(&fields, &root->index->index->ident->name);
        array_push(&types, &root->index->object->type->name);
        root = root->index->object;
    }
    struct field_info rfi;
    rfi.offset = 0;
    rfi.root_struct = root;
    for(int i = array_size(&fields) - 1; i >= 0; i--){
        symbol field_name = *(symbol*)array_get(&fields, i);
        symbol type_name = *(symbol*)array_get(&types, i);
        struct field_info fi = sc_get_field_info(sc, type_name, field_name);
        rfi.offset += fi.offset;
        rfi.align = fi.align;
        rfi.type = fi.type;
    }
    assert(rfi.type->name == index->index->index->type->name);
    array_deinit(&fields);
    array_deinit(&types);
    return rfi;
}
