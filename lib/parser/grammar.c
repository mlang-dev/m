/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * grammar parser
 *
 */

#include "parser/grammar.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "parser/meta_grammar.h"

const char *keyword_symbols[] = {
    "import",
    "extern",
    "type",
    "if",
    "else",
    "then",
    "in",
    "for",
    "unary",
    "binary",
    "..",
    "...",
    "true",
    "false",
    "||",
    "&&",
    "!",
    "%",
    "&",
    "^",
    "*",
    "/",
    "+",
    "-",
    "<",
    "=",
    ">",
    "<=",
    ">=",
    "==",
    "!=",
    "?",
    "@",
    "|",
    ":",
    "(",
    ")",
    "[",
    "]"
};

int keyword_count = ARRAY_SIZE(keyword_symbols);

struct grammar *grammar_new(const char *grammar_text)
{
    (void)grammar_text;
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashtable_init(&grammar->rules);
    grammar->start_symbol = 0;
    struct grammar *meta_grammar = create_meta_grammar();
    free_meta_grammar(meta_grammar);
    return grammar;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rules);
    free(grammar);
}