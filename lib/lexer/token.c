#include "lexer/token.h"
#include <assert.h>
/*

    {"?", TOKEN_SYMBOL},
    {"@", TOKEN_SYMBOL},
 
 
*/
struct token_pattern token_patterns[END_TOKENS] = {
    {0, TOKEN_NULL},
    {"\0", TOKEN_EOF},
    {0, TOKEN_INDENT},
    {0, TOKEN_DEDENT},
    {"\n", TOKEN_NEWLINE},
    {"[0-9]+", TOKEN_INT},
    {"[+-]?([0-9]*[.])?[0-9]+", TOKEN_FLOAT},
    {"[_a-zA-Z][_a-zA-Z0-9]*", TOKEN_IDENT},
    {0, TOKEN_CHAR}, 
    {0, TOKEN_STRING},
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
    {"(", TOKEN_LPAREN},
    {")", TOKEN_RPAREN},
    {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},
    {"{", TOKEN_LCBRACKET},
    {"}", TOKEN_RCBRACKET},
    {"unary", TOKEN_UNOPDEF},
    {"binary", TOKEN_BINOPDEF},

    {0, TOKEN_SYMBOL},
    {0, TOKEN_OP},
    {".", TOKEN_DOT},
    {"..", TOKEN_RANGE},
    {"...", TOKEN_VARIADIC},
    {"=", TOKEN_ASSIGN},
    {":", TOKEN_ISTYPEOF},
    {0, TOKEN_TOTAL},

    {"||", OP_OR},
    {"&&", OP_AND},
    {"!", OP_NOT},

    {"|", OP_BOR}, 
    {"&", OP_BAND},
   
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
/*
const char *get_opcode(int opcode)
{
    assert(opcode > TOKEN_TOTAL && opcode < END_TOKENS);
    return token_patterns[opcode].pattern;
}
*/