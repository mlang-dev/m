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

struct link_list_entry {
    list_entry(link_list_entry) list;
    void *data;
};

list_head(link_list, link_list_entry);

typedef struct symboltable {
    struct hashtable ht;
    /*
     * TODO: symbols are a stack data structure, we might need a version with array implementation 
     * for performance improvement with more effective memory cache
     */
    struct link_list symbols;
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
