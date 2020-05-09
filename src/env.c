/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include <stdlib.h>
#include "llvm-c/Core.h"

#include "env.h"

struct menv *env_new()
{
    struct menv* env = malloc(sizeof(*env));
    env->context = LLVMContextCreate();
    env->type_env = type_env_new(env->context);
    return env;
}

void env_free(struct menv* env)
{
    LLVMContextRef context = (LLVMContextRef)env->context;
    LLVMContextDispose(context);
    type_env_free(env->type_env);
    free(env);
}
