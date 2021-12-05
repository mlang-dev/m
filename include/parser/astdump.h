/*
 * astdump.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dump ast tree
 */
#ifndef __MLANG_ASTDUMP_H__
#define __MLANG_ASTDUMP_H__

#include "parser/ast.h"
#include "clib/string.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

string dump(struct ast_node* node);

#ifdef __cplusplus
}
#endif

#endif
