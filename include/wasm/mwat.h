/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for wat code gen
 */
#ifndef __MLANG_MWAT_H__
#define __MLANG_MWAT_H__
#include "clib/string.h"

#ifdef __cplusplus
extern "C" {
#endif

string parse_exp_as_module(const char *expr);


#ifdef __cplusplus
}
#endif

#endif