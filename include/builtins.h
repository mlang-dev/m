/*
 * builtins.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for builtin functions
 */
#ifndef __MLANG_BUILTIN_H__
#define __MLANG_BUILTIN_H__

#include "ast.h"

void get_builtins(void* context, std::vector<exp_node*>& builtins);

#endif