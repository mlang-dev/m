#include "util.h"
#include <string>
#include <cstdio>
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"

std::string make_unique_name(const char* name){
    static int i = 0;
    char s[16];
    std::string str = s;
    return str;
}

std::string make_function_name(std::string name) {
    if (!name.length())
        return make_unique_name("anon_fun_");
    return name;
}

void dump(void* p){
    
}
// void dump(llvm::Function *p){
//     //p->dump(); //this feature requires cmake to build llvm with -DLLVM_ENABLE_DUMP=ON
//     p->print(llvm::dbgs(), nullptr);
// }

// void dump(llvm::Module *p){
//     //p->print(llvm::dbgs(), nullptr);
// }
