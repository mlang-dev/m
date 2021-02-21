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
#include "codegen.h"
#include "env.h"

#ifdef __cplusplus
extern "C" {
#endif


struct type_exp* retrieve(struct env* env, const char* name);
struct type_exp* analyze(struct env* env, struct exp_node* node);
bool is_builtin(struct env* env, const char* name);

#ifdef __cplusplus
}
#endif

#endif
