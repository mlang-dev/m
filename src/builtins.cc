/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Built-in functions 
 */

#include "builtins.h"
#include "util.h"
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
            args.push_back(format("arg%d", i));
        }
    }

    std::vector<std::string> names = split(name.c_str(), '.');
    //log(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    return create_prototype_node(nullptr, { 1, 0 }, names.back().c_str(), args);
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