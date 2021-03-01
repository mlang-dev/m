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

struct link_list_entry {
    list_entry(link_list_entry) list;
    void* data;
};

list_head(link_list, link_list_entry);

typedef struct symboltable{
    struct hashtable ht;    
    struct link_list symbols;
} symboltable;

void symboltable_init(symboltable* st);
void symboltable_deinit(symboltable* st);
void symboltable_push(symboltable* st, symbol s, void* data);
symbol symboltable_pop(symboltable* st);
void* symboltable_get(symboltable* st, symbol s);
bool has_symbol(symboltable* st, symbol s);


#ifdef __cplusplus
}
#endif

#endif
