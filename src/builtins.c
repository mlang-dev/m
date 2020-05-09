/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Built-in functions 
 */
#include <string.h>
#include <stdlib.h>

#include "builtins.h"
#include "clib/util.h"
#include "clib/string.h"

#include "llvm-c/Core.h"


const char * buiiltin_funs[] = {
    "llvm.sin",
    "llvm.cos",
    "llvm.sqrt",
};

struct prototype_node* _create_for_id(void* pcontext, const char* name)
{
    LLVMContextRef context = (LLVMContextRef)pcontext;
    LLVMTypeRef types[1] = {LLVMDoubleTypeInContext(context)};
    unsigned id = LLVMLookupIntrinsicID(name, strlen(name));
    LLVMTypeRef fun = LLVMIntrinsicGetType(context, id, types, 1);
    size_t param_count = LLVMCountParamTypes(fun);
    LLVMTypeRef *params = malloc(param_count * sizeof(LLVMTypeRef));
    LLVMGetParamTypes(fun, params);
    //;llvm::Intrinsic::getName(id, types);
    array args;
    array_string_init(&args);
    for (size_t i = 0; i < param_count; i++) {
        string arg = str_format("arg%d", i);
        array_push(&args, &arg);
    }
    string str_name;
    string_init_chars(&str_name, name);
    array names = string_split(&str_name, '.');
    //log_info(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    struct source_loc loc = {1, 0};
    struct prototype_node* node = create_prototype_node_default(0, loc, 
        string_get(STRING_POINTER(array_back(&names))), &args);
    string_deinit(&str_name);
    array_deinit(&names);
    //array_deinit(&args);
    free(params);
    return node;
    //return 0;
}

array get_builtins(void* context)
{
    array builtins;
    array_init(&builtins, sizeof(struct exp_node*));
    int builtins_num = sizeof(buiiltin_funs)/sizeof(char*);
    for (int i = 0; i < builtins_num; i++) {
        struct prototype_node *proto = _create_for_id(context, buiiltin_funs[i]);
        array_push(&builtins, &proto);
    }
    array args; //array of string
    string str;
    array_string_init(&args);
    string_init_chars(&str, "char");
    array_push(&args, &str);
    struct source_loc loc = {1, 0};
    struct prototype_node* proto = create_prototype_node_default(0, loc, "print", &args);
    array_push(&builtins, &proto);
    //args copied to the prototype node, so not needed to deinit
    return builtins;
}
