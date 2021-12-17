#include "lexer/token.h"
#include <assert.h>

struct token_pattern token_patterns[PATTERN_COUNT] = {
    {0, TOKEN_NULL, OP_NULL},
    {"\0", TOKEN_EOF, OP_NULL},
    {0, TOKEN_INDENT, OP_NULL},
    {0, TOKEN_DEDENT, OP_NULL},
    {"\n", TOKEN_NEWLINE, OP_NULL},
    {"[0-9]+", TOKEN_INT, OP_NULL},
    {"[+-]?([0-9]*[.])?[0-9]+", TOKEN_FLOAT, OP_NULL},
    {"[_a-zA-Z][_a-zA-Z0-9]*", TOKEN_IDENT, OP_NULL},
    {0, TOKEN_CHAR, OP_NULL}, 
    {0, TOKEN_STRING, OP_NULL},
    {"import", TOKEN_IMPORT, OP_NULL},
    {"extern", TOKEN_EXTERN, OP_NULL},
    {"type", TOKEN_TYPE, OP_NULL},
    {"if", TOKEN_IF, OP_NULL},
    {"then", TOKEN_THEN, OP_NULL},
    {"else", TOKEN_ELSE, OP_NULL},
    {"true", TOKEN_TRUE, OP_NULL},
    {"false", TOKEN_FALSE, OP_NULL},
    {"in", TOKEN_IN, OP_NULL},
    {"for", TOKEN_FOR, OP_NULL},
    {"(", TOKEN_LPAREN, OP_NULL},
    {")", TOKEN_RPAREN, OP_NULL},
    {"[", TOKEN_LBRACKET, OP_NULL},
    {"]", TOKEN_RBRACKET, OP_NULL},
    {"{", TOKEN_LCBRACKET, OP_NULL},
    {"}", TOKEN_RCBRACKET, OP_NULL},

    {",", TOKEN_COMMA, OP_NULL},

    {".", TOKEN_DOT, OP_NULL},
    {"..", TOKEN_RANGE, OP_NULL},
    {"...", TOKEN_VARIADIC, OP_NULL},
    {"=", TOKEN_ASSIGN, OP_NULL},
    {":", TOKEN_ISTYPEOF, OP_NULL},

    {0, TOKEN_OP, OP_NULL},
    {"||", TOKEN_OP, OP_OR},
    {"&&", TOKEN_OP, OP_AND},
    {"!", TOKEN_OP, OP_NOT},

    {"|", TOKEN_OP, OP_BOR}, 
    {"&", TOKEN_OP, OP_BAND},
   
    {"^", TOKEN_OP, OP_EXPO},
    {"*", TOKEN_OP, OP_TIMES},
    {"/", TOKEN_OP, OP_DIVISION},
    {"%", TOKEN_OP, OP_MODULUS},
    {"+", TOKEN_OP, OP_PLUS},
    {"-", TOKEN_OP, OP_MINUS},

    {"<", TOKEN_OP, OP_LT},
    {"<=", TOKEN_OP, OP_LE},
    {"==", TOKEN_OP, OP_EQ},
    {">", TOKEN_OP, OP_GT},
    {">=", TOKEN_OP, OP_GE},
    {"!=", TOKEN_OP, OP_NE},
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
    struct token_patterns tps = {token_patterns, PATTERN_COUNT};
    return tps;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < PATTERN_COUNT);
    return token_patterns[(int)TOKEN_OP + (int)opcode].pattern;
}
