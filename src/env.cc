/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include <stdlib.h>
#include "llvm-c/Core.h"

#include "env.h"

menv *env_new()
{
    menv* env = (menv*)malloc(sizeof(menv));
    env->context = LLVMContextCreate();
    return env;
}

void env_free(menv* env)
{
    LLVMContextRef context = (LLVMContextRef)env->context;
    LLVMContextDispose(context);
    free(env);
}
