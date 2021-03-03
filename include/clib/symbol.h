/*
 * symbol.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol c header file
 * symbol represents the pointer to string object, it's stored as global string constant for performance
 * improvement in symbol table(hash for the pointer or integer is fast then string)
 */
#ifndef __CLIB_SYMBOL_H__
#define __CLIB_SYMBOL_H__

#include <stdbool.h>
#include <stddef.h>

#include "clib/hashtable.h"
#include "clib/string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef string *symbol;

symbol to_symbol(const char *name);
void symbols_init();
void symbols_deinit();

#ifdef __cplusplus
}
#endif

#endif
