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

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    struct type_env* type_env;
    struct parser* parser;
};

struct env* env_new(bool is_repl);
void env_free(struct env* env);

#ifdef __cplusplus
}
#endif

#endif
