#include "util.h"
#include <cstdio>

std::string MakeUniqueName(const char* name){
    static int i = 0;
    char s[16];
    sprintf(s, "%s%d", name, i++);
    std::string str = s;
    return str;
}

std::string MakeFunctionName(std::string name) {
    if (!name.length())
        return MakeUniqueName("anon_fun_");
    return name;
}