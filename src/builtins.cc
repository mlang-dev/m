/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Built-in functions 
 */

#include "builtins.h"
#include "clib/util.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Intrinsics.h"

llvm::Intrinsic::ID builtin_ids[] = {
    llvm::Intrinsic::IndependentIntrinsics::sin,
    llvm::Intrinsic::IndependentIntrinsics::cos,
    llvm::Intrinsic::IndependentIntrinsics::sqrt,
};
#define NUM_BUILTINS 3

prototype_node* _create_for_id(void* context, llvm::Intrinsic::ID id)
{
    llvm::ArrayRef<llvm::Type*> types;
    std::string name = llvm::Intrinsic::getName(id, types);
    llvm::SmallVector<llvm::Intrinsic::IITDescriptor, 8> iitds;
    llvm::Intrinsic::getIntrinsicInfoTableEntries(id, iitds);
    llvm::ArrayRef<llvm::Intrinsic::IITDescriptor> iitdsRef = iitds;
    std::vector<std::string> args;
    for (int i = 0; i < iitdsRef.size(); i++) {
        if (iitdsRef[i].Kind == llvm::Intrinsic::IITDescriptor::Argument && iitdsRef[i].getArgumentKind() != llvm::Intrinsic::IITDescriptor::ArgKind::AK_MatchType) {
            string arg = format("arg%d", i);
            args.push_back(std::string(arg.data));
            string_deinit(&arg);
        }
    }
    string str_name;
    string_init_chars(&str_name, name.c_str());
    array names = string_split(&str_name, '.');
    //log_info(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    prototype_node* node = create_prototype_node(nullptr, { 1, 0 }, 
        STRING_POINTER(array_get(&names, names.size-1))->data, args);
    string_deinit(&str_name);
    array_deinit(&names);
    return node;
    //return 0;
}

void get_builtins(void* context, std::vector<exp_node*>& builtins)
{
    for (int i = 0; i < NUM_BUILTINS; i++) {
        builtins.push_back((exp_node*)_create_for_id(context, builtin_ids[i]));
    }
    std::vector<std::string> args;
    args.push_back("char");
    builtins.push_back((exp_node*)create_prototype_node(nullptr, { 1, 0 }, "print", args));
}