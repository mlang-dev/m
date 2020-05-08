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
extern "C"{
#endif

typedef struct _type_env{
    struct hashtable type_env;  //hashtable of <string, type_exp*>
    array nogens; //array of type_exp*
}type_env;

type_exp* retrieve(type_env* env, const char *name);
type_env* type_env_new(void* context);
void type_env_free(type_env* env);
type_exp* analyze(type_env* env, struct exp_node* node);

#ifdef __cplusplus
}
#endif

#endif
