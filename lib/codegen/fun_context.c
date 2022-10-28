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
    struct_type_init(&fc->stack_type);
    fc->stack_size_info = (struct type_size_info){ 0, 0, 0, 0 };
}

void fc_deinit(struct fun_context *fc)
{
    struct_type_deinit(&fc->stack_type);
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

u32 fc_get_stack_offset(struct fun_context *fc, struct ast_node *node)
{
    struct var_info *vi = fc_get_var_info(fc, node);
    if(vi->alloc_index<0) return 0;
    return *(u64*)array_get(&fc->stack_size_info.sl->field_offsets, vi->alloc_index) / 8;
}

struct struct_layout *fc_get_stack_sl(struct fun_context *fc, struct ast_node *node)
{
    struct var_info *vi = fc_get_var_info(fc, node);
    if(vi->alloc_index<0) return 0;
    return *(struct struct_layout**)array_get(&fc->stack_size_info.sl->field_layouts, vi->alloc_index);
}

int fc_register_alloc(struct fun_context *fc, struct type_expr *type)
{
    assert(type->kind == KIND_OPER);
    struct_type_add_member(&fc->stack_type, type);
    return array_size(&fc->stack_type.args) - 1;
}

u32 fc_get_stack_size(struct fun_context *fc)
{
    
    fc->stack_size_info = get_type_size_info(&fc->stack_type);
    u32 stack_size = fc->stack_size_info.width_bits / 8;
    //clang-wasm ABI always uses 16 bytes alignment
    stack_size = align_to(stack_size, 16);
    return stack_size;
   
   
}
