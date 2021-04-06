#include "codegen/fun_info.h"
#include "codegen/codegen.h"
#include "codegen/x86_64_abi.h"
#include "sys.h"

void fun_info_init(struct fun_info *fi)
{
    fi->is_chain_call = false;
    array_init(&fi->args, sizeof(struct ast_abi_arg));
}

void fun_info_deinit(struct fun_info *fi)
{
    array_deinit(&fi->args);
}

struct fun_info *get_fun_info(symbol fun_name, struct type_oper *fun_type)
{
    struct hashtable *fun_infos = get_fun_infos();
    struct fun_info *result = hashtable_get_p(fun_infos, fun_name);
    if (result)
        return result;
    struct fun_info fi;
    fun_info_init(&fi);
    fi.ret.type = &fun_type->base;
    struct ast_abi_arg aa;
    for (unsigned i = 0; i < array_size(&fun_type->args); i++) {
        aa.type = *(struct type_exp **)array_get(&fun_type->args, i);
        array_push(&fi.args, &aa);
    }
    x86_64_update_abi_info(&fi);
    hashtable_set_p(fun_infos, fun_name, &fi);
    return (struct fun_info *)hashtable_get_p(fun_infos, fun_name);
}
