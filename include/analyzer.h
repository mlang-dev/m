/*
 * analyzer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for type inference & semantic analsysis
 */
#ifndef __MLANG_ANALYZER_H__
#define __MLANG_ANALYZER_H__

#include "ast.h"
#include "clib/hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct type_env {
    struct hashtable type_env; //hashtable of <string, struct type_exp*>
    struct array nogens; //struct array of struct type_exp*
    struct array builtins;
};

struct type_exp* retrieve(struct type_env* env, const char* name);
struct type_env* type_env_new(void* context);
void type_env_free(struct type_env* env);
struct type_exp* analyze(struct type_env* env, struct exp_node* node);

#ifdef __cplusplus
}
#endif

#endif
