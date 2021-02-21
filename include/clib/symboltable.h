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
#include "clib/symbol.h"
#include "clib/list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hashtable symboltable;

struct link_list_entry {
    list_entry(link_list_entry) list;
    void* data;
};

list_head(link_list, link_list_entry);

void symboltable_init(symboltable* symbol_table);
void symboltable_deinit(symboltable* symbol_table);
void symboltable_add(symboltable* symbol_table, symbol symbol, void* data);
void* symboltable_get(symboltable* symbol_table, symbol symbol);
void symboltable_pop(symboltable* symbol_table, symbol symbol);

#ifdef __cplusplus
}
#endif

#endif
