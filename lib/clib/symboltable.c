/*
 * symboltable.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * symbol table c file
 * symboltable represents hashtable of symbol (key) and list of pointers
 */
#include "clib/symbol.h"
#include "clib/symboltable.h"
#include "clib/util.h"

void symboltable_init(symboltable *st)
{
    hashtable_init(&st->ht);
    st->symbols.first = NULL;
}

void symboltable_deinit(symboltable *st)
{
    hashtable_deinit(&st->ht);
}

link_list_add_data_fn(symbol_list, symbol_list_entry, void *)

link_list_remove_data_fn(symbol_list, symbol_list_entry, void*)

void _symboltable_remove(symboltable *st, symbol s)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    symbol_list_remove_data_from_head(ll);
}

void symboltable_push(symboltable *st, symbol s, void *data)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    if (!ll) {
        MALLOC(ll, sizeof(*ll));
        ll->first = NULL;
        hashtable_set_p(&st->ht, s, ll);
    }
    symbol_list_add_data_to_head(ll, data);
    /* add the symbol to symbols stack*/
    symbol_list_add_data_to_head(&st->symbols, s);
    //printf("push symbol: %s\n", string_get(s));
}

symbol symboltable_pop(symboltable *st)
{
    symbol s = symbol_list_remove_data_from_head(&st->symbols);
    if (!s) {
        return s;
    }
    _symboltable_remove(st, s);
    //printf("pop symbol: %s\n", string_get(s));
    return s;
}

void *symboltable_get(symboltable *st, symbol s)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    if (!ll || !ll->first) {
        return NULL;
    }
    return ll->first->data;
}

bool has_symbol(symboltable *st, symbol s)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    return ll && ll->first;
}

bool has_symbol_in_scope(symboltable *st, symbol s, symbol end_s)
{
    struct symbol_list_entry *entry;
    list_foreach(entry, &st->symbols)
    {
        if (entry->data == end_s)
            break;
        else if (entry->data == s)
            return true;
    }
    return false;
}
