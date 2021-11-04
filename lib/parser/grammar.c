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

const char *grammar_symbols[] = {
    "::=",
    "|",
    "{",
    "}"
};

int grammar_symbols_count = ARRAY_SIZE(grammar_symbols);

struct grammar *grammar_new(const char *grammar_text)
{
    (void)grammar_text;
    struct grammar *grammar = 0;
    MALLOC(grammar, sizeof(*grammar));
    hashtable_init(&grammar->rules);
    grammar->start_symbol = 0;
    struct tokenizer *tokenizer = create_tokenizer_for_string(grammar_text, grammar_symbols, grammar_symbols_count);
    struct token *tok;
    do {
        tok = get_token(tokenizer);
        //printf("got token: %s\n", token_type_strings[tok->token_type]);
    }while(tok->token_type != TOKEN_EOF);
    destroy_tokenizer(tokenizer);
    return grammar;
}

void grammar_free(struct grammar *grammar)
{
    hashtable_deinit(&grammar->rules);
    free(grammar);
}