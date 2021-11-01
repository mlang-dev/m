/*
 * earley_parser.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement a Earley parser
 */
#include "parser/earley_parser.h"
#include "clib/array.h"
#include "parser/grammar.h"

struct earley_parser *earley_parser_new(const char *grammar_text)
{
    struct grammar *grammer = grammar_new(grammar_text);
    grammar_free(grammer);
    struct earley_parser *parser;
    MALLOC(parser, sizeof(*parser));
    return 0;
}

void earley_parser_free(struct earley_parser *parser)
{
    free(parser);
}
