#include "lexer/token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name) {0, 0, pattern, TOKEN_##tok_name, OP_##op_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name) {#tok_name, 0, pattern, TOKEN_##tok_name, OP_##op_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name) {keyword, 0, keyword, TOKEN_##tok_name, OP_##op_name}

struct token_pattern token_patterns[PATTERN_COUNT] = {
    NULL_PATTERN(0, NULL, NULL),
    TOKEN_PATTERN("\0", EOF, NULL),
    TOKEN_PATTERN(0, INDENT, NULL),
    TOKEN_PATTERN(0, DEDENT, NULL),
    TOKEN_PATTERN("\n", NEWLINE, NULL),
    TOKEN_PATTERN("[0-9]+", INT, NULL),
    TOKEN_PATTERN("[+-]?([0-9]*[.])?[0-9]+", FLOAT, NULL),
    TOKEN_PATTERN("[_a-zA-Z][_a-zA-Z0-9]*", IDENT, NULL),
    TOKEN_PATTERN(0, CHAR, NULL), 
    TOKEN_PATTERN(0, STRING, NULL),

    KEYWORD_PATTERN("import", IMPORT, NULL),
    KEYWORD_PATTERN("extern", EXTERN, NULL),
    KEYWORD_PATTERN("type", TYPE, NULL),
    KEYWORD_PATTERN("if", IF, NULL),
    KEYWORD_PATTERN("then", THEN, NULL),
    KEYWORD_PATTERN("else", ELSE, NULL),
    KEYWORD_PATTERN("true", TRUE, NULL),
    KEYWORD_PATTERN("false", FALSE, NULL),
    KEYWORD_PATTERN("in", IN, NULL),
    KEYWORD_PATTERN("for", FOR, NULL),
    KEYWORD_PATTERN("(", LPAREN, NULL),
    KEYWORD_PATTERN(")", RPAREN, NULL),
    KEYWORD_PATTERN("[", LBRACKET, NULL),
    KEYWORD_PATTERN("]", RBRACKET, NULL),
    KEYWORD_PATTERN("{", LCBRACKET, NULL),
    KEYWORD_PATTERN("}", RCBRACKET, NULL),

    KEYWORD_PATTERN(",", COMMA, NULL),

    KEYWORD_PATTERN(".", DOT, NULL),
    KEYWORD_PATTERN("..", RANGE, NULL),
    KEYWORD_PATTERN("...", VARIADIC, NULL),
    KEYWORD_PATTERN("=", ASSIGN, NULL),
    KEYWORD_PATTERN(":", ISTYPEOF, NULL),

    TOKEN_PATTERN(0, OP, NULL),
    KEYWORD_PATTERN("||", OP, OR),
    KEYWORD_PATTERN("&&", OP, AND),
    KEYWORD_PATTERN("!", OP, NOT),

    KEYWORD_PATTERN("|", OP, BOR), 
    KEYWORD_PATTERN("&", OP, BAND),
   
    KEYWORD_PATTERN("^", OP, EXPO),
    KEYWORD_PATTERN("*", OP, TIMES),
    KEYWORD_PATTERN("/", OP, DIVISION),
    KEYWORD_PATTERN("%", OP, MODULUS),
    KEYWORD_PATTERN("+", OP, PLUS),
    KEYWORD_PATTERN("-", OP, MINUS),

    KEYWORD_PATTERN("<", OP, LT),
    KEYWORD_PATTERN("<=", OP, LE),
    KEYWORD_PATTERN("==", OP, EQ),
    KEYWORD_PATTERN(">", OP, GT),
    KEYWORD_PATTERN(">=", OP, GE),
    KEYWORD_PATTERN("!=", OP, NE),
};

struct hashtable token_patterns_by_symbol;

const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

void token_init()
{
    hashtable_init(&token_patterns_by_symbol);
    for(int i = 0; i < PATTERN_COUNT; i++){
        if(token_patterns[i].name){
            token_patterns[i].symbol_name = to_symbol2_0(token_patterns[i].name);
            hashtable_set_p(&token_patterns_by_symbol, token_patterns[i].symbol_name, &token_patterns[i]);
        }
    }
}

void token_deinit()
{
    hashtable_deinit(&token_patterns_by_symbol);
}

struct token_patterns get_token_patterns()
{
    struct token_patterns tps = {token_patterns, PATTERN_COUNT};
    return tps;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < OP_TOTAL);
    return get_token_pattern_by_opcode(opcode)->pattern;
}

symbol get_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode)
{
    assert(opcode >= 0 && opcode < OP_TOTAL);
    if (token_type == TOKEN_OP)
        return get_token_pattern_by_opcode(opcode)->symbol_name;
    else
        return get_token_pattern_by_token_type(token_type)->symbol_name;
}

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < PATTERN_COUNT);
    return &token_patterns[(int)TOKEN_OP + (int)opcode];
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type > 0 && token_type <= TOKEN_OP);
    return &token_patterns[(int)token_type];
}

struct token_pattern *get_token_pattern_by_symbol(symbol symbol)
{
    return (struct token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}
