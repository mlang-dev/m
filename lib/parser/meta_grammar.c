#include "parser/meta_grammar.h"
#include "clib/util.h"
#include "clib/win/libfmemopen.h"
#include "lexer/lexer.h"
#include <stdio.h>

const char *meta_grammar_text = "\
grammar = rules EOF\n\
rules   = rules rule | rule\n\
rule    = IDENT = exprs NEWLINE\n\
exprs   = exprs | expr | expr\n\
expr    = expr atom | atom\n\
atom    = IDENT | STRING";

struct grammar *create_meta_grammar()
{
    // FILE *file = fmemopen((void *)meta_grammar_text, strlen(meta_grammar_text), "r");
    // struct tokenizer *tokenizer = create_tokenizer(file, "", 0, 0);
    struct grammar *meta_grammar;
    MALLOC(meta_grammar, sizeof(*meta_grammar));
    hashtable_init_with_value_size(&meta_grammar->rules, sizeof(struct rule), 0);
    //struct tokenizer *tokenizer = create_tokenizer_for_string(meta_grammar_text, 0, 0);
    //struct token *token = get_token(tokenizer);
    //destroy_tokenizer(tokenizer);
    return meta_grammar;
}

void free_meta_grammar(struct grammar *meta_grammar)
{
    hashtable_deinit(&meta_grammar->rules);
    free(meta_grammar);
}
