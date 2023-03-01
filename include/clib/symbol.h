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
symbol to_symbol2(const char *name, size_t name_size);
symbol string_2_symbol(string *name);
symbol get_temp_symbol();
extern symbol EmptySymbol;
void symbols_init();
void symbols_deinit();

#ifdef __cplusplus
}
#endif

#endif
