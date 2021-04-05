#include "codegen/fun_info.h"
#include "codegen/codegen.h"
#include "codegen/x86_64_abi.h"
#include "sys.h"

struct fun_info *get_fun_info(struct call_node *call)
{
    symbol callee = call->specialized_callee? call->specialized_callee : call->callee;
    struct hashtable *fun_infos = get_fun_infos();
    struct fun_info *result = hashtable_get_p(fun_infos, callee);
    if(result) return result;
    struct fun_info fi;
    fi.is_chain_call = false;
    array_init(&fi.args, sizeof(struct ast_abi_arg));
    fi.ret.node = &call->base;
    struct ast_abi_arg aa;
    for(unsigned i=0; i<array_size(&call->args); i++){
        aa.node = *(struct exp_node**)array_get(&call->args, i);
        array_push(&fi.args, &aa);
    }
    x86_64_update_abi_info(&fi);
    hashtable_set_p(fun_infos, callee, &fi);
    return (struct fun_info*)hashtable_get_p(fun_infos, callee);
}

void clear_fun_info(struct hash_entry* entry)
{
    struct fun_info *fi = (struct fun_info*)(entry->data.key_value_pair + sizeof(void*));
    array_deinit(&fi->args);
}

