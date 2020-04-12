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
#include <string>
#include <vector>
#include "parser.h"

using namespace std;
string dump(exp_node* node);

#endif