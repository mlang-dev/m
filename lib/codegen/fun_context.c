#include "codegen/fun_context.h"
#include "codegen/type_size_info.h"


void fun_context_init(struct fun_context *fc)
{
    fc->fun = 0;
    fc->local_vars = 0;
    fc->local_params = 0;
    fc->local_sp = 0;
    symboltable_init(&fc->varname_2_index);
    hashtable_init(&fc->ast_2_index);
    hashtable_init_with_value_size(&fc->ast_2_alloc_index, sizeof(int), 0);
    array_init(&fc->allocs, sizeof(struct mem_alloc));
}

void fun_context_deinit(struct fun_context *fc)
{
    array_deinit(&fc->allocs);
    hashtable_deinit(&fc->ast_2_alloc_index);
    hashtable_deinit(&fc->ast_2_index);
    symboltable_deinit(&fc->varname_2_index);
}

int fun_alloc_memory(struct fun_context *fc, struct ast_node *block, bool save_original_copy)
{
    struct mem_alloc alloc;
    u64 block_size = 0;
    u32 block_align = 0;
    u32 num_nodes = array_size(&block->block->nodes);
    if(!num_nodes) return -1;
    for(size_t i = 0; i < num_nodes; i++){
        struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, i);
        u64 size = get_type_size(node->type) / 8;
        if (size < 4) size = 4;
        if(size > block_align){
            block_align = size;
        }
        block_size = align_to(block_size, size);
        block_size += size;  
    }
    block_size = save_original_copy? align_to(block_size * 2, block_align) : align_to(block_size, block_align);
    alloc.size = block_size;
    alloc.mem_type = Stack;
    alloc.address = 0;
    alloc.align = block_align;
    array_push(&fc->allocs, &alloc);
    return array_size(&fc->allocs) - 1;
}

u32 get_stack_size(struct fun_context *fc)
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
