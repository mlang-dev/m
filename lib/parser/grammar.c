/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "util.h"

struct grammar *grammar_new(const char *grammar_text)
{
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashtable_init(&grammar->rules);
    grammar->start_symbol = 0;

    return grammar;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rules);
    free(grammar);
}