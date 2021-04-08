/*
 * analyzer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for type inference & semantic analsysis
 */
#ifndef __MLANG_ANALYZER_H__
#define __MLANG_ANALYZER_H__

#include "clib/symbol.h"
#include "codegen/codegen.h"
#include "parser/ast.h"
#include "sema/sema_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct type_exp *retrieve_type_for_var_name(struct sema_context *env, symbol name);
struct type_exp *analyze_and_generate_builtin_codes(struct sema_context *env, struct exp_node *node);
struct type_exp *analyze(struct sema_context *env, struct exp_node *node);

#ifdef __cplusplus
}
#endif

#endif
