/*
 * symboltable.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol table c file
 * symboltable represents hashtable of symbol (key) and list of pointers
 */

#include "clib\symbol.h"
#include "clib\symboltable.h"

void symboltable_init(symboltable* symbol_table){
    hashtable_init(symbol_table);
}

void symboltable_deinit(symboltable* symbol_table){
    hashtable_deinit(symbol_table);
}

void symboltable_add(symboltable* symbol_table, symbol symbol, void* data){
    hashtable_set_p(symbol_table, symbol, data);
}

void* symboltable_get(symboltable* symbol_table, symbol symbol){
    return hashtable_get_p(symbol_table, symbol);
}

void symboltable_pop(symboltable* symbol_table, symbol symbol){
    
}
