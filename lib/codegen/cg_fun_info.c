#include "codegen/cg_fun_info.h"
#include "sys.h"

struct cg_fun_info *get_cg_fun_info(struct call_node *call)
{
    struct cg_fun_info *fi;
    MALLOC(fi, sizeof(*fi));
    fi->is_chain_call = false;
    array_init(&fi->args, sizeof(struct ast_abi_arg));
    fi->ret.node = &call->base;
    struct ast_abi_arg aa;
    for(unsigned i=0; i<array_size(&call->args); i++){
        aa.node = *(struct exp_node**)array_get(&call->args, i);
        array_push(&fi->args, &aa);
    }
    return fi;
}

void free_cg_fun_info(struct cg_fun_info *fi)
{
    array_deinit(&fi->args);
    free(fi);
}

