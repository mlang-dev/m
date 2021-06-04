#include "parser/meta_grammar.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include <stdio.h>

// const char *meta_grammar_text = R"(
// grammar = rules EOF
// rules   = rules rule | rule
// rule    = IDENT "=" exprs NEWLINE
// exprs   = exprs "|" expr | expr
// expr    = expr atom | atom
// atom    = IDENT | STRING
// )";
struct grammar *create_meta_grammar()
{
    // FILE *file = fmemopen((void *)meta_grammar_text, strlen(meta_grammar_text), "r");
    // struct tokenizer *tokenizer = create_tokenizer(file, "", 0, 0);
    struct grammar *meta_grammar;
    MALLOC(meta_grammar, sizeof(*meta_grammar));
    hashtable_init_with_value_size(&meta_grammar->rules, sizeof(struct rule), 0);
    struct rule rule;
    rule.nonterm = to_symbol("grammar");
    array_init(&rule.exprs, sizeof(struct expr));
    return meta_grammar;
}

void free_meta_grammar(struct grammar *meta_grammar)
{
    hashtable_deinit(&meta_grammar->rules);
    free(meta_grammar);
}
