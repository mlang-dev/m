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

void symboltable_init(symboltable *st)
{
    hashtable_init(&st->ht);
    st->symbols.first = NULL;
}

void symboltable_deinit(symboltable *st)
{
    hashtable_deinit(&st->ht);
}

void _add_to_head(struct link_list *ll, void *data)
{
    struct link_list_entry *entry = malloc(sizeof(*entry));
    entry->data = data;
    entry->list.next = NULL;
    list_insert_head(ll, entry, list);
}

void *_remove_from_head(struct link_list *ll)
{
    void *data = 0;
    if (!ll) {
        return data;
    }
    if (ll->first) {
        struct link_list_entry *first = ll->first;
        data = first->data;
        list_remove_head(ll, list);
        free(first);
    }
    return data;
}

void _symboltable_remove(symboltable *st, symbol s)
{
    struct link_list *ll = hashtable_get_p(&st->ht, s);
    _remove_from_head(ll);
}

void symboltable_push(symboltable *st, symbol s, void *data)
{
    struct link_list *ll = hashtable_get_p(&st->ht, s);
    if (!ll) {
        ll = malloc(sizeof(*ll));
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
    struct link_list *ll = hashtable_get_p(&st->ht, s);
    if (!ll || !ll->first) {
        return NULL;
    }
    return ll->first->data;
}

bool has_symbol(symboltable *st, symbol s)
{
    struct link_list *ll = hashtable_get_p(&st->ht, s);
    return ll && ll->first;
}

bool has_symbol_in_scope(symboltable *st, symbol s, symbol end_s)
{
    struct link_list_entry *entry;
    list_foreach(entry, &st->symbols, list)
    {
        if (entry->data == end_s)
            break;
        else if (entry->data == s)
            return true;
    }
    return false;
}
