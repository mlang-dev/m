#include "lexer/token.h"

/*
IDENT ::= [_a-zA-Z][_a-zA-Z0-9]{0,30}
*/
const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

void token_init(struct token *token)
{
    token->token_type = TOKEN_NULL;
    token->loc.line = 0;
    token->loc.col = 0;
}
