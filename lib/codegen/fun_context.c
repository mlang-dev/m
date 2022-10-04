#include "codegen/fun_context.h"

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
