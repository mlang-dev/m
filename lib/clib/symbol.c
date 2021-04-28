/*
 * symbol.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol c file
 * symbol represents the pointer to string object, it's stored as global string
 * constant for performance improvement in symbol table(hash for the pointer or
 * integer is fast then string)
 */
#include "clib/symbol.h"

#include <assert.h>
#include <stdlib.h>

struct hashtable *g_symbols = 0;
symbol EmptySymbol = 0;

symbol to_symbol(const char *name)
{
    assert(g_symbols);
    symbol sym = (symbol)hashtable_get(g_symbols, name);
    if (!sym) {
        sym = string_new(name);
        hashtable_set(g_symbols, name, sym);
    }
    return sym;
}

symbol string_2_symbol(string *name)
{
    return to_symbol(string_get(name));
}

void symbols_init()
{
    if (g_symbols)
        return;
    g_symbols = malloc(sizeof(*g_symbols));
    hashtable_init(g_symbols);
    EmptySymbol = to_symbol("");
}

void symbols_deinit()
{
    if (!g_symbols)
        return;
    hashtable_deinit(g_symbols);
    free(g_symbols);
    g_symbols = NULL;
}