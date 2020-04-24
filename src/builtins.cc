/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Built-in functions 
 */
#include "builtins.h"
#include "clib/util.h"
#include "clib/string.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm-c/Core.h"

llvm::Intrinsic::ID builtin_ids[] = {
    llvm::Intrinsic::IndependentIntrinsics::sin,
    llvm::Intrinsic::IndependentIntrinsics::cos,
    llvm::Intrinsic::IndependentIntrinsics::sqrt,
};
#define NUM_BUILTINS 3

prototype_node* _create_for_id(void* pcontext, llvm::Intrinsic::ID id)
{
    LLVMContextRef context = (LLVMContextRef)pcontext;
    LLVMTypeRef types[1] = {LLVMDoubleTypeInContext(context)};
    size_t name_len = 0;
    std::string name = LLVMIntrinsicCopyOverloadedName(id, NULL, 0, &name_len);
    LLVMTypeRef fun = LLVMIntrinsicGetType(context, id, types, 1);
    size_t param_count = LLVMCountParamTypes(fun);
    LLVMTypeRef *params = (LLVMTypeRef*)malloc(param_count * sizeof(LLVMTypeRef));
    LLVMGetParamTypes(fun, params);
    //;llvm::Intrinsic::getName(id, types);
    array args;
    array_string_init(&args);
    for (int i = 0; i < param_count; i++) {
        string arg = str_format("arg%d", i);
        array_push(&args, &arg.base);
    }
    string str_name;
    string_init_chars(&str_name, name.c_str());
    array names = string_split(&str_name, '.');
    //log_info(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    prototype_node* node = create_prototype_node(nullptr, { 1, 0 }, 
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
    array_init(&builtins, sizeof(exp_node*));
    for (int i = 0; i < NUM_BUILTINS; i++) {
        prototype_node *proto = _create_for_id(context, builtin_ids[i]);
        array_push(&builtins, &proto);
    }
    array args; //array of string
    string str;
    array_string_init(&args);
    string_init_chars(&str, "char");
    array_push(&args, (object*)&str);
    prototype_node* proto = create_prototype_node(nullptr, { 1, 0 }, "print", &args);
    array_push(&builtins, &proto);
    string_deinit(&str);
    //args copied to the prototype node, so not needed to deinit
    return builtins;
}