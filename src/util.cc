#include "util.h"
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"

static char alpha_nums[36];
static bool alpha_nums_init = false;

int random(int min, int max){
    return min + (rand() % static_cast<int>(max - min + 1));
}

std::string make_unique_name(const char* name){
    if (!alpha_nums_init){
        char c = '0';
        for(int i=0;i<10;i++){
            alpha_nums[i] = c + i;
        }
        c = 'a';
        for(int i=0; i<26; i++){
            alpha_nums[i+10] = c + i;
        }
        alpha_nums_init = true;
    }
    static int i = 0;
    char s[16];
    for(int i=0;i<16;i++){
        int j = random(0, 35);
        s[i] = alpha_nums[j];
    }
    std::string str = s;
    return str + name;
}

void* log(LogLevel level, const char * string_format, ...) {
    va_list args;
    char format[512];
    sprintf(format, "%s: %s\n", LogLevelString[level], string_format);
    va_start(args, string_format);
    vfprintf(stderr, format, args);
    va_end(args);
    return 0;
}

std::vector<std::string> split(std::string str, char separator){
    std::vector<std::string> strings;
    std::istringstream f(str);
    std::string s;    
    while (getline(f, s, separator)) {
        strings.push_back(s);
    }
    return strings;
}

std::string format(const char * string_format, ...) {
    va_list args;
    char data[512];
    va_start(args, string_format);
    vsprintf(data, string_format, args);
    va_end(args);
    return data;
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
