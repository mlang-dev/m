#include "codegen/ast_abi_arg.h"
#include "sys.h"

struct cg_fun_args *create_cg_fun_args(struct call_node *call)
{
    struct cg_fun_args *fa;
    MALLOC(fa, sizeof(*fa));
    array_init(&fa->args, sizeof(struct ast_abi_arg));
    fa->ret.node = &call->base;
    struct ast_abi_arg aa;
    for(unsigned i=0; i<array_size(&call->args); i++){
        aa.node = *(struct exp_node**)array_get(&call->args, i);
        array_push(&fa->args, &aa);
    }
    return fa;
}

void free_cg_fun_args(struct cg_fun_args *fa)
{
    array_deinit(&fa->args);
    free(fa);
}

