/*
 * eval.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for ast node eval, this is primarily used for compile time function evaluation. first version of
 * the implementation is to calculate address value at compile time
 */
#ifndef __MLANG_EVAL_H__
#define __MLANG_EVAL_H__

#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

int eval(struct ast_node *node);

#ifdef __cplusplus
}
#endif

#endif
