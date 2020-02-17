#include "builtins.h"
#include "llvm/IR/Intrinsics.h"
#include "util.h"


llvm::Intrinsic::ID builtin_ids[] = {
    llvm::Intrinsic::ID::sin,
    llvm::Intrinsic::ID::cos,
    llvm::Intrinsic::ID::sqrt
};
#define NUM_BUILTINS 3

prototype_node* _create_for_id(void* context, llvm::Intrinsic::ID id){
    std::string name = llvm::Intrinsic::getName(id);
    llvm::SmallVector<llvm::Intrinsic::IITDescriptor, 8> iitds;
    llvm::Intrinsic::getIntrinsicInfoTableEntries(id, iitds); 
    llvm::ArrayRef<llvm::Intrinsic::IITDescriptor> iitdsRef = iitds;
    std::vector<std::string> args;
    for(int i =0; i<iitdsRef.size(); i++){
        if (iitdsRef[i].Kind == llvm::Intrinsic::IITDescriptor::Argument && iitdsRef[i].getArgumentKind() != llvm::Intrinsic::IITDescriptor::ArgKind::AK_MatchType){
            args.push_back(format("arg%d", i));
        }
    }
    
    std::vector<std::string> names = split(name, '.');
    //log(DEBUG, "get func: %d, name: %s", id, names.back().c_str());
    return create_prototype_node(nullptr, {1, 0}, names.back(), args);
    //return 0;
}

void get_builtins(void* context, std::vector<exp_node*> &builtins){
    for (int i=0; i<NUM_BUILTINS; i++){
        builtins.push_back((exp_node*)_create_for_id(context, builtin_ids[i]));
    }
}