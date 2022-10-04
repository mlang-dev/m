#include "codegen/fun_context.h"
#include "codegen/type_size_info.h"


void fun_context_init(struct fun_context *fc)
{
    fc->fun = 0;
    fc->local_vars = 0;
    fc->local_params = 0;
    symboltable_init(&fc->varname_2_index);
    hashtable_init(&fc->ast_2_index);
    array_init(&fc->allocs, sizeof(struct mem_alloc));
}

void fun_context_deinit(struct fun_context *fc)
{
    array_deinit(&fc->allocs);
    hashtable_deinit(&fc->ast_2_index);
    symboltable_deinit(&fc->varname_2_index);
}

void fun_alloc_memory(struct fun_context *fc, struct ast_node *block, bool save_original_copy)
{
    struct mem_alloc alloc;
    u64 block_size = 0;
    for(size_t i = 0; i < array_size(&block->block->nodes); i++){
        struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, i);
        u64 size = get_type_size(node->type);
        if (size < 4) size = 4;
        block_size = align_to(block_size, size);
        block_size += size;  //clang-wasm ABI always uses 16 bytes alignment
    }
    block_size = save_original_copy? align_to(block_size * 2, 16) : align_to(block_size, 16);
    alloc.size = block_size;
    alloc.mem_type = Stack;
    alloc.address = 0;
    array_push(&fc->allocs, &alloc);
}
