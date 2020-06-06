/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m environment, a top class to store all data for compiling session
 */
#include <llvm-c/Core.h>
#include <stdlib.h>

#include "env.h"

struct menv* env_new(bool is_repl)
{
    struct menv* env = malloc(sizeof(*env));
    env->parser = parser_new(is_repl);
    env->type_env = type_env_new(env->parser);
    return env;
}

void env_free(struct menv* env)
{
    type_env_free(env->type_env);
    parser_free(env->parser);
    free(env);
}
