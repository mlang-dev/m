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
    env->type_sys = type_env_new(env->context);
    return env;
}

void env_free(menv* env)
{
    LLVMContextRef context = (LLVMContextRef)env->context;
    LLVMContextDispose(context);
    type_env_free(env->type_sys);
    free(env);
}
