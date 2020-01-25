#include "util.h"
#include <string>
#include <cstdio>
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"

std::string make_unique_name(const char* name){
    static int i = 0;
    char s[16];
    std::string str = s;
    return str + name;
}

void* log(LogLevel level, const char * string_format, ...) {
    va_list args;
    char format[512];
    sprintf(format, "%s: %s", LogLevelString[level], string_format);
    va_start(args, string_format);
    fprintf(stderr, format, args );
    va_end(args);
    return 0;
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
