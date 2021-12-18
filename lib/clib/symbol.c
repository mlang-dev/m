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

symbol to_symbol2_0(const char *name)
{
    return to_symbol(name);
}


symbol string_2_symbol(string *name)
{
    return to_symbol(string_get(name));
}

symbol string_2_symbol2(string *name)
{
    return to_symbol2(string_get(name), string_size(name));
}

void symbols_init()
{
    if (g_symbols)
        return;
    MALLOC(g_symbols, sizeof(*g_symbols));
    hashtable_c_str_key_init(g_symbols);
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
