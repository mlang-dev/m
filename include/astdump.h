/*
 * astdump.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dump ast tree
 */
#ifndef __MLANG_ASTDUMP_H__
#define __MLANG_ASTDUMP_H__

#include <stdio.h>
#include "ast.h"
#include "clib/string.h"

#ifdef __cplusplus
extern "C"{
#endif

string dump(exp_node* node);

#ifdef __cplusplus
}
#endif

#endif