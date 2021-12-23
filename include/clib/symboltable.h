/*
 * symboltable.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol c header file
 * symboltable represents hashtable of symbol (key) and list of pointers
 * 
 */
#ifndef __CLIB_SYMBOLTABLE_H__
#define __CLIB_SYMBOLTABLE_H__

#include <stdbool.h>
#include <stddef.h>

#include "clib/hashtable.h"
#include "clib/list.h"
#include "clib/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

link_list(symbol_list, symbol_list_entry, void*)

typedef struct symboltable {
    struct hashtable ht;
    /*
     * TODO: symbols are a stack data structure, we might need a version with array implementation 
     * for performance improvement with more effective memory cache
     */
    struct symbol_list symbols;
} symboltable;

void symboltable_init(symboltable *st);
void symboltable_deinit(symboltable *st);
void symboltable_push(symboltable *st, symbol s, void *data);
symbol symboltable_pop(symboltable *st);
void *symboltable_get(symboltable *st, symbol s);
bool has_symbol(symboltable *st, symbol s);
bool has_symbol_in_scope(symboltable *st, symbol s, symbol end_s);

#ifdef __cplusplus
}
#endif

#endif
