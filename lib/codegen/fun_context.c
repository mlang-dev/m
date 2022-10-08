#include "codegen/fun_context.h"
#include "codegen/type_size_info.h"
#include <assert.h>


void fc_init(struct fun_context *fc)
{
    fc->fun = 0;
    fc->local_vars = 0;
    fc->local_params = 0;
    fc->local_sp = 0;
    symboltable_init(&fc->varname_2_index);
    hashtable_init(&fc->ast_2_index);
    array_init(&fc->allocs, sizeof(struct mem_alloc));
}

void fc_deinit(struct fun_context *fc)
{
    for(size_t i = 0; i < array_size(&fc->allocs); i++){
        struct mem_alloc *alloc = array_get(&fc->allocs, i);
        if(alloc->sl && !alloc->sl->type_name){
            sl_free(alloc->sl);
        }
    }
    array_deinit(&fc->allocs);
    hashtable_deinit(&fc->ast_2_index);
    symboltable_deinit(&fc->varname_2_index);
}

struct var_info *_fc_get_var_info_by_node(struct fun_context *fc, struct ast_node *node)
{
    struct var_info *vi = (struct var_info *)hashtable_get_p(&fc->ast_2_index, node);
    assert(vi);
    return vi;
}

struct var_info *_fc_get_var_info_by_varname(struct fun_context *fc, symbol varname)
{
    struct var_info *vi = symboltable_get(&fc->varname_2_index, varname);
    assert(vi);
    return vi;
}

struct var_info *fc_get_var_info(struct fun_context *fc, struct ast_node *node)
{
    struct var_info *vi;
    if (node->node_type == IDENT_NODE)
        vi = _fc_get_var_info_by_varname(fc, node->ident->name);
    else if (node->node_type == VAR_NODE)
        vi = _fc_get_var_info_by_varname(fc, node->var->var_name);
    else
        vi = _fc_get_var_info_by_node(fc, node);
    assert(vi);
    return vi;
}

struct mem_alloc *fc_get_alloc(struct fun_context *fc, struct ast_node *node)
{
    struct var_info *vi = fc_get_var_info(fc, node);
    if(vi->alloc_index<0) return 0;
    return array_get(&fc->allocs, vi->alloc_index);
}

int fc_register_alloc(struct fun_context *fc, struct type_expr *struct_type)
{
    assert(struct_type->kind == KIND_OPER);
    struct type_size_info tsi = get_type_size_info(struct_type);
    struct mem_alloc alloc;
    alloc.size = tsi.width_bits / 8;
    alloc.mem_type = Stack;
    alloc.address = 0;
    alloc.align = tsi.align_bits / 8;
    alloc.sl = tsi.sl;
    array_push(&fc->allocs, &alloc);
    return array_size(&fc->allocs) - 1;
}

u32 fc_get_stack_size(struct fun_context *fc)
{
    size_t num_allocs = array_size(&fc->allocs);
    u32 stack_size = 0;
    for(size_t i = 0; i < num_allocs; i++){
        struct mem_alloc *alloc = array_get(&fc->allocs, num_allocs - 1 - i);
        alloc->address = stack_size;
        stack_size += alloc->size;
    }
    //clang-wasm ABI always uses 16 bytes alignment
    stack_size = align_to(stack_size, 16);
    return stack_size;
}
