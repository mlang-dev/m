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
#include "parser/ast.h"
#include "sema/sema_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct type_expr *retrieve_type_for_var_name(struct sema_context *env, symbol name);
struct type_expr *analyze(struct sema_context *env, struct ast_node *node);
symbol get_type_symbol(enum type type_enum);

#ifdef __cplusplus
}
#endif

#endif
