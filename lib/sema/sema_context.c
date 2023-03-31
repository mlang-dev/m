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
#include <assert.h>
#include <limits.h>

size_t enter_scope(struct sema_context *context)
{
    symboltable_push(&context->varname_2_typexprs, context->scope_marker, 0);
    return ++context->scope_level;
}

size_t leave_scope(struct sema_context *context)
{
    symbol s;
    do {
        s = symboltable_pop(&context->varname_2_typexprs);
        assert(s);
    } while (s != context->scope_marker);
    return --context->scope_level;
}

void _free_nested_levels(void *arr)
{
    struct array *array = arr;
    array_deinit(array);
}

struct sema_context *sema_context_new(struct hashtable *symbol_2_int_types, struct ast_node *stdio, struct ast_node *math, bool is_repl)
{
    struct sema_context *context;
    CALLOC(context, 1, sizeof(*context));
    context->symbol_2_int_types = symbol_2_int_types;
    context->is_repl = is_repl;
    array_init(&context->nongens, sizeof(struct type_item *));
    array_init(&context->used_builtin_names, sizeof(symbol));
    symboltable_init(&context->typename_2_typexpr_pairs);
    symboltable_init(&context->varname_2_typexprs);
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
    hashtable_init(&context->type_2_ref_symbol);
    context->scope_level = 0;
    context->scope_marker = to_symbol("<enter_scope_marker>");
    /*nullary type: builtin default types*/
    for (size_t i = 0; i < TYPE_TYPES; i++) {
        symbol type_name = get_type_symbol(i);
        struct type_item *te;
        if(i != TYPE_ARRAY){
            te = create_nullary_type(i);
        } else {
            struct array dims;
            array_init(&dims, sizeof(u32));
            te = create_array_type(create_unit_type(), &dims);
        }
        struct type_item_pair *tep = get_type_item_pair(te->name);
        push_symbol_type(&context->typename_2_typexpr_pairs, type_name, tep);
        hashtable_set_p(&context->type_2_ref_symbol, type_name, to_ref_symbol(type_name));
    }

    struct array builtins;
    array_init(&builtins, sizeof(struct ast_node *));
    if (stdio){
        for (size_t i = 0; i < array_size(&stdio->block->nodes); i++) {
            struct ast_node *node = array_get_ptr(&stdio->block->nodes, i);
            array_push(&builtins, &node);
        }
    }
    if (math){
        array_add(&builtins, &math->block->nodes);
    }
    for (size_t i = 0; i < array_size(&builtins); i++) {
        struct ast_node *node = array_get_ptr(&builtins, i);
        assert(node->node_type == FUNC_TYPE_NODE);
        analyze(context, node);
        push_symbol_type(&context->varname_2_typexprs, node->ft->name, node->type);
        hashtable_set_p(&context->builtin_ast, node->ft->name, node);
    }
    array_init_free(&context->nested_levels, sizeof(struct array), _free_nested_levels);
    context->builtin_ast_block = block_node_new(&builtins);
    return context;
}

void sema_context_free(struct sema_context *context)
{
    array_deinit(&context->nested_levels);
    hashtable_deinit(&context->struct_typename_2_asts);
    hashtable_deinit(&context->specialized_ast);
    hashtable_deinit(&context->generic_ast);
    hashtable_deinit(&context->builtin_ast);
    hashtable_deinit(&context->func_types);
    hashtable_deinit(&context->calls);
    hashtable_deinit(&context->type_2_ref_symbol);
    stack_deinit(&context->func_stack);
    hashtable_deinit(&context->gvar_name_2_ast);
    symboltable_deinit(&context->varname_2_asts);
    symboltable_deinit(&context->varname_2_typexprs);
    symboltable_deinit(&context->typename_2_typexpr_pairs);
    array_deinit(&context->used_builtin_names);
    array_deinit(&context->nongens);
    array_deinit(&context->new_specialized_asts);
    node_free(context->builtin_ast_block);
    FREE(context);
}

symbol get_ref_type_symbol(struct sema_context *context, symbol type_name)
{
    symbol ref_name = hashtable_get_p(&context->type_2_ref_symbol, type_name);
    if(!ref_name){
        ref_name = to_ref_symbol(type_name);
        hashtable_set_p(&context->type_2_ref_symbol, type_name, ref_name);
    }
    return ref_name;
}

struct ast_node *find_generic_fun(struct sema_context *context, symbol fun_name)
{
    return hashtable_get(&context->generic_ast, string_get(fun_name));
}

struct source_location zero_loc = {0, 0, 0, 0};

struct ast_node *_sc_struct_get_offset_expr(struct sema_context *sc, struct type_item *aggr_type, struct ast_node *field_node)
{
    struct ast_node *struct_node = hashtable_get_p(&sc->struct_typename_2_asts, aggr_type->name);
    assert(struct_node->type->kind == KIND_OPER);
    int index = find_field_index(struct_node, field_node);
    struct struct_layout *sl = get_type_size_info(struct_node->type).sl;
    u32 offset = *(u64 *)array_get(&sl->field_offsets, index) / 8;
    return int_node_new(offset, zero_loc);
}

struct ast_node *_binary_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct type_item *type, struct source_location loc)
{
    struct ast_node *node = binary_node_new(opcode, lhs, rhs, loc);
    node->type = type;
    return node;
}

struct ast_node *_sc_array_get_offset_expr(struct sema_context *sc, struct type_item *aggr_type, struct ast_node *field_expr)
{
    u32 subarray_size = get_type_size(aggr_type->val_type) / 8;
    for(u32 i = 1; i < array_size(&aggr_type->dims); i++){
        subarray_size *= *(u32*)array_get(&aggr_type->dims, i);
    }
    return _binary_node_new(OP_STAR, int_node_new(subarray_size, zero_loc), field_expr, field_expr->type, zero_loc);
}

struct ast_node *sc_aggr_get_offset_expr(struct sema_context *sc, struct type_item *aggr_type, struct ast_node *field_node)
{
    if(aggr_type->type == TYPE_ARRAY)
        return _sc_array_get_offset_expr(sc, aggr_type, field_node);
    else if(is_adt(aggr_type))
        return _sc_struct_get_offset_expr(sc, aggr_type, field_node);
    assert(false);
}

//each reference of aggr type will have a field_info returned
void sc_get_field_infos_from_root(struct sema_context *sc, struct ast_node* index, struct array *field_infos)
{
    assert(index->node_type == MEMBER_INDEX_NODE);
    struct ast_node *root = index;
    struct array field_accessors;
    array_init(&field_accessors, sizeof(struct type_item*));
    //x.y.z turned into z.y.x
    while(root->node_type == MEMBER_INDEX_NODE){
        array_push(&field_accessors, &root);
        root = root->index->object;
    }
    struct field_info *rfi;
    int end = array_size(&field_accessors) - 1;
    for(int i = end; i >= 0; i--){
        struct ast_node *field_accessor = array_get_p(&field_accessors, i);
        struct ast_node *aggr_node = field_accessor->index->object;
        struct ast_node *field_node = field_accessor->index->index;
        struct type_item *aggr_type = aggr_node->type;
        if(i == end || aggr_type->type == TYPE_REF){
            struct field_info root_fi;
            array_push(field_infos, &root_fi);
            rfi = (struct field_info *)array_back(field_infos);
            rfi->offset_expr = int_node_new(0, zero_loc);
            rfi->aggr_root = aggr_node;
            if(aggr_type->type == TYPE_REF){
                aggr_type = aggr_type->val_type;
            }
        }
        struct ast_node *offset_expr;
        switch(aggr_type->type){
        default:
            assert(false);
            break;
        case TYPE_TUPLE:
        case TYPE_STRUCT:
        case TYPE_VARIANT:
            {
                offset_expr = sc_aggr_get_offset_expr(sc, aggr_type, field_node);
                break;
            }
        case TYPE_ARRAY:
            {
                offset_expr = sc_aggr_get_offset_expr(sc, aggr_type, node_copy(field_node));
                break;
            }
        }
        rfi->offset_expr = _binary_node_new(OP_PLUS, rfi->offset_expr, offset_expr, offset_expr->type, zero_loc);
        rfi->align = get_type_align(field_accessor->type);//fi.align;
        rfi->type = field_accessor->type;// fi.type;
    }
    array_deinit(&field_accessors);
}

struct array *_get_func_block(struct sema_context *context)
{
    if(array_size(&context->nested_levels) == 0){
        return 0;
    }
    return array_back(&context->nested_levels);
}

struct loop_nested_level *get_current_block_level(struct sema_context *context)
{
    struct array *loop = _get_func_block(context);
    if(!loop) return 0;
    if(!array_size(loop)) return 0;
    return array_back(loop);
}

void enter_function(struct sema_context *context)
{
    struct array bnls;
    array_init(&bnls, sizeof(struct loop_nested_level));
    array_push(&context->nested_levels, &bnls);
}

void leave_function(struct sema_context *context)
{
    struct array *bnls = array_pop(&context->nested_levels);
    array_deinit(bnls);
}

struct loop_nested_level *enter_loop(struct sema_context *context)
{
    struct array *loops = _get_func_block(context);
    if(!loops) return 0;
    struct loop_nested_level bnl = { 0 };
    array_push(loops, &bnl);
    return array_back(loops);
}

void leave_loop(struct sema_context *context)
{
    struct array *loops = _get_func_block(context);
    if(loops) array_pop(loops);
}
