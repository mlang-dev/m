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

void _add_to_head(struct symbol_list *ll, void *data)
{
    struct symbol_list_entry *entry;
    MALLOC(entry, sizeof(*entry));
    entry->data = data;
    entry->list.next = NULL;
    link_list_insert_head(ll, entry);
}

void *_remove_from_head(struct symbol_list *ll)
{
    void *data = 0;
    if (!ll) {
        return data;
    }
    if (ll->first) {
        struct symbol_list_entry *first = ll->first;
        data = first->data;
        list_remove_head(ll, list);
        FREE(first);
    }
    return data;
}

void _symboltable_remove(symboltable *st, symbol s)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    _remove_from_head(ll);
}

void symboltable_push(symboltable *st, symbol s, void *data)
{
    struct symbol_list *ll = hashtable_get_p(&st->ht, s);
    if (!ll) {
        MALLOC(ll, sizeof(*ll));
        ll->first = NULL;
        hashtable_set_p(&st->ht, s, ll);
    }
    _add_to_head(ll, data);
    /* add the symbol to symbols stack*/
    _add_to_head(&st->symbols, s);
    //printf("push symbol: %s\n", string_get(s));
}

symbol symboltable_pop(symboltable *st)
{
    symbol s = _remove_from_head(&st->symbols);
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
    list_foreach(entry, &st->symbols, list)
    {
        if (entry->data == end_s)
            break;
        else if (entry->data == s)
            return true;
    }
    return false;
}
