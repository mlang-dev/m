#include <string>
#include "llvm/IR/Function.h"

std::string MakeUniqueName(const char* root);
std::string MakeFunctionName(std::string name);
void dump(llvm::Function *p);
void dump(llvm::Module *p);

