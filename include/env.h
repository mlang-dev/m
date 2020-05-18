/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "analyzer.h"
#include "codegen.h"

#ifdef __cplusplus
extern "C" {
#endif

struct menv {
    struct type_env* type_env;
    struct parser* parser;
    struct code_generator* cg;
};

struct menv* env_new(const char* file_name, bool is_repl, FILE* file);
void env_free(struct menv* env);

#ifdef __cplusplus
}
#endif

#endif
