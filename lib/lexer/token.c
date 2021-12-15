#include "lexer/token.h"

struct token_pattern token_patterns[END_TOKENS] = {
    {0, TOKEN_NULL},
    {"\0", TOKEN_EOF},
    {"\n", TOKEN_NEWLINE},
    {"[_a-zA-Z][_a-zA-Z0-9]*", TOKEN_IDENT},
    {"[0-9]+", TOKEN_INT},
    {"[+-]?([0-9]*[.])?[0-9]+", TOKEN_FLOAT},
    {0, TOKEN_CHAR}, 
    {0, TOKEN_STRING},
    {0, TOKEN_OP},
    {"import", TOKEN_IMPORT},
    {"extern", TOKEN_EXTERN},
    {"type", TOKEN_TYPE},
    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"else", TOKEN_ELSE},
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    {"in", TOKEN_IN},
    {"for", TOKEN_FOR},
    {".", TOKEN_DOT},
    {"..", TOKEN_RANGE},
    {"...", TOKEN_VARIADIC},
    {"=", TOKEN_ASSIGN},
    {0, TOKEN_INDENT},
    {0, TOKEN_DEDENT},
    {0, TOKEN_SYMBOL},
    {0, TOKEN_TOTAL},

    {"||", OP_OR},
    {"&&", OP_AND},
    {"!", OP_NOT},
    {"^", OP_EXPO},
    {"*", OP_TIMES},
    {"/", OP_DIVISION},
    {"%", OP_MODULUS},
    {"+", OP_PLUS},
    {"-", OP_MINUS},

    {"<", OP_LT},
    {"<=", OP_LE},
    {"==", OP_EQ},
    {">", OP_GT},
    {">=", OP_GE},
    {"!=", OP_NE},
};

const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

void token_init(struct token *token)
{
    token->token_type = TOKEN_NULL;
    token->loc.line = 0;
    token->loc.col = 0;
}

struct token_patterns get_token_patterns()
{
    struct token_patterns tps = {token_patterns, END_TOKENS};
    return tps;
}