/*
 * symbol.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol c file
 * symbol represents the pointer to string object, it's stored as global string
 * constant for performance improvement in symbol table(hash for the pointer or
 * integer is faster then string)
 */
#include "clib/symbol.h"
#include <assert.h>
#include "clib/util.h"

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

symbol to_symbol2(const char *name, size_t name_size)
{
    assert(g_symbols);
    symbol sym = (symbol)hashtable_get2(g_symbols, name, name_size);
    if (!sym) {
        sym = string_new2(name, name_size);
        hashtable_set2(g_symbols, name, name_size, sym);
    }
    return sym;
}

symbol string_2_symbol(string *name)
{
    char *str = string_get(name);
    return to_symbol(str);
}

void _free_symbol(void *string)
{
    symbol symbol = string;
    string_deinit(symbol);
    FREE(symbol);
}

void symbols_init()
{
    if (g_symbols)
        return;
    MALLOC(g_symbols, sizeof(*g_symbols));
    hashtable_c_str_key_init(g_symbols, _free_symbol);
    EmptySymbol = to_symbol("");
}

void symbols_deinit()
{
    if (!g_symbols)
        return;
    hashtable_deinit(g_symbols);
    FREE(g_symbols);
    g_symbols = NULL;
}
