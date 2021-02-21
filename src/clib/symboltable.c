/*
 * symboltable.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol table c file
 * symboltable represents hashtable of symbol (key) and list of pointers
 */
#include <stdlib.h>

#include "clib/symbol.h"
#include "clib/symboltable.h"

void symboltable_init(symboltable* symbol_table){
    hashtable_init(symbol_table);
}

void symboltable_deinit(symboltable* symbol_table){
    hashtable_deinit(symbol_table);
}

void symboltable_push(symboltable* symbol_table, symbol symbol, void* data){
    struct link_list *ll = hashtable_get_p(symbol_table, symbol);
    if(!ll){
        ll = malloc(sizeof(*ll));
        ll->first = NULL;
        hashtable_set_p(symbol_table, symbol, ll);
    }
    struct link_list_entry* entry = malloc(sizeof(*entry));
    entry->data = data;
    entry->list.next = NULL;
    list_insert_head(ll, entry, list);
}

void* symboltable_get(symboltable* symbol_table, symbol symbol){
    struct link_list* ll = hashtable_get_p(symbol_table, symbol);
    if (!ll||!ll->first){
        return NULL;
    }
    return ll->first->data;
}

void symboltable_pop(symboltable* symbol_table, symbol symbol){
    struct link_list* ll = hashtable_get_p(symbol_table, symbol);
    if (!ll){
        return;
    }
    if(ll->first){
        struct link_list_entry* first = ll->first;
        list_remove_head(ll, list);
        free(first);
    }
}
