/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Built-in functions 
 */
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "clib/string.h"
#include "clib/util.h"

#include "llvm-c/Core.h"

const char* buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

struct prototype_node* _create_for_id(void* pcontext, const char* name)
{
    LLVMContextRef context = (LLVMContextRef)pcontext;
    LLVMTypeRef types[1] = { LLVMDoubleTypeInContext(context) };
    unsigned id = LLVMLookupIntrinsicID(name, strlen(name));
    LLVMTypeRef fun = LLVMIntrinsicGetType(context, id, types, 1);
    size_t param_count = LLVMCountParamTypes(fun);
    LLVMTypeRef* params = malloc(param_count * sizeof(LLVMTypeRef));
    LLVMGetParamTypes(fun, params);
    //;llvm::Intrinsic::getName(id, types);
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    for (size_t i = 0; i < param_count; i++) {
        fun_param.var_name = str_format("arg%d", i);      
        fun_param.base.annotated_type = (struct type_exp*)create_nullary_type(TYPE_DOUBLE);
        array_push(&fun_params, &fun_param);
    }
    string str_name;
    string_init_chars(&str_name, name);
    struct array names = string_split(&str_name, '.');
    //log_info(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    struct source_loc loc = { 1, 0 };
    struct prototype_node* node = create_prototype_node_default(0, loc,
        string_get(STRING_POINTER(array_back(&names))), &fun_params, (struct type_exp*)create_nullary_type(TYPE_DOUBLE));
    string_deinit(&str_name);
    array_deinit(&names);
    free(params);
    return node;
    //return 0;
}

struct array get_builtins(void* context)
{
    struct array builtins;
    array_init(&builtins, sizeof(struct exp_node*));
    int builtins_num = ARRAY_SIZE(buiiltin_funs);
    for (int i = 0; i < builtins_num; i++) {
        struct prototype_node* proto = _create_for_id(context, buiiltin_funs[i]);
        array_push(&builtins, &proto);
    }
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    string_init_chars(&fun_param.var_name, "char");
    array_push(&fun_params, &fun_param);
    struct source_loc loc = { 1, 0 };
    struct prototype_node* proto = create_prototype_node_default(0, loc, "print", &fun_params, (struct type_exp*)create_nullary_type(TYPE_UNIT));
    array_push(&builtins, &proto);
    //args copied to the prototype node, so not needed to deinit
    return builtins;
}
