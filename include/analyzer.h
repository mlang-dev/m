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
#include "codegen.h"
#include "env.h"
#include "clib/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif


struct type_exp* retrieve_type_for_var_name(struct env* env, const char* name);
struct type_exp* analyze_and_generate_code(struct env* env, struct exp_node* node);
struct type_exp* analyze(struct env* env, struct exp_node* node);

#ifdef __cplusplus
}
#endif

#endif
