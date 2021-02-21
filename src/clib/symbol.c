/*
 * symbol.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol c file
 * symbol represents the pointer to string object, it's stored as global string constant for performance
 * improvement in symbol table(hash for the pointer or integer is fast then string)
 */

#include "clib\symbol.h"

symbol to_symbol(symbols* symbols, const char* name){
    symbol sym = (symbol)hashtable_get(symbols, name);
    if(!sym){
        sym = string_new(name);
        hashtable_set(symbols, name, sym);
    }
    return sym;
}