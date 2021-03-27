/*
 * analyzer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for type inference & semantic analsysis
 */
#ifndef __MLANG_ANALYZER_H__
#define __MLANG_ANALYZER_H__

#include "parser/ast.h"
#include "codegen/codegen.h"
#include "sema/env.h"
#include "clib/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct type_exp* retrieve_type_for_var_name(struct env* env, symbol name);
struct type_exp* analyze_and_generate_code(struct env* env, struct exp_node* node);
struct type_exp* analyze(struct env* env, struct exp_node* node);
struct type_size_info get_type_size_info(struct env *env, struct type_exp *type);

#ifdef __cplusplus
}
#endif

#endif
