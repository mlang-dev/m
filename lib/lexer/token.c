#include "lexer/token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name) {0, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name) {#tok_name, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name) {keyword, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}
#define NAME_KEYWORD_PATTERN(name, keyword, tok_name, op_name) {name, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name}

struct token_pattern token_patterns[TERMINAL_COUNT] = {
    TOKEN_PATTERN(0, EOF, NULL), // 0
    TOKEN_PATTERN(0, EPSILON, NULL),
    TOKEN_PATTERN(0, INDENT, NULL),
    TOKEN_PATTERN(0, DEDENT, NULL),
    TOKEN_PATTERN("\n", NEWLINE, NULL),
    TOKEN_PATTERN("[0-9]+", INT, NULL), // 5
    TOKEN_PATTERN("([0-9]*.)?[0-9]+", FLOAT, NULL),

    TOKEN_PATTERN(0, CHAR, NULL),
    TOKEN_PATTERN(0, STRING, NULL),

    KEYWORD_PATTERN("import", IMPORT, NULL),
    KEYWORD_PATTERN("extern", EXTERN, NULL), // 10
    KEYWORD_PATTERN("type", TYPE, NULL),
    KEYWORD_PATTERN("let", LET, NULL),
    KEYWORD_PATTERN("if", IF, NULL),
    KEYWORD_PATTERN("then", THEN, NULL),
    KEYWORD_PATTERN("else", ELSE, NULL), // 15
    KEYWORD_PATTERN("true", TRUE, NULL),
    KEYWORD_PATTERN("false", FALSE, NULL),
    KEYWORD_PATTERN("in", IN, NULL),
    KEYWORD_PATTERN("for", FOR, NULL),

    TOKEN_PATTERN("[_a-zA-Z][_a-zA-Z0-9]*", IDENT, NULL), // 20

    NAME_KEYWORD_PATTERN("(", "\\(", LPAREN, NULL),
    NAME_KEYWORD_PATTERN(")", "\\)", RPAREN, NULL),
    NAME_KEYWORD_PATTERN("[", "\\[", LBRACKET, NULL),
    NAME_KEYWORD_PATTERN("]", "\\]", RBRACKET, NULL),
    KEYWORD_PATTERN("{", LCBRACKET, NULL), // 25
    KEYWORD_PATTERN("}", RCBRACKET, NULL),

    KEYWORD_PATTERN(",", COMMA, NULL),

    NAME_KEYWORD_PATTERN("..", "\\.\\.", RANGE, NULL),
    NAME_KEYWORD_PATTERN("...", "\\.\\.\\.", VARIADIC, NULL), // 30
    KEYWORD_PATTERN("=", ASSIGN, NULL),
    KEYWORD_PATTERN(":", ISTYPEOF, NULL),

    TOKEN_PATTERN(0, OP, NULL),
    NAME_KEYWORD_PATTERN(".", "\\.", OP, DOT), // literal dot
    NAME_KEYWORD_PATTERN("||", "\\|\\|", OP, OR),
    KEYWORD_PATTERN("&&", OP, AND), // 35
    KEYWORD_PATTERN("!", OP, NOT),

    KEYWORD_PATTERN("~", OP, BNOT),
    NAME_KEYWORD_PATTERN("|", "\\|", OP, BOR),
    KEYWORD_PATTERN("^", OP, BEOR),
    KEYWORD_PATTERN("&", OP, BAND),
    KEYWORD_PATTERN("<<", OP, BSL),
    KEYWORD_PATTERN(">>", OP, BSR),

    // KEYWORD_PATTERN("^", OP, EXPO),
    NAME_KEYWORD_PATTERN("**", "\\*\\*", OP, EXPO), // 40
    NAME_KEYWORD_PATTERN("*", "\\*", OP, TIMES), // 40
    KEYWORD_PATTERN("/", OP, DIVISION),
    KEYWORD_PATTERN("%", OP, MODULUS),
    NAME_KEYWORD_PATTERN("+", "\\+", OP, PLUS),
    KEYWORD_PATTERN("-", OP, MINUS),

    KEYWORD_PATTERN("<", OP, LT), // 45
    KEYWORD_PATTERN("<=", OP, LE),
    KEYWORD_PATTERN("==", OP, EQ),
    KEYWORD_PATTERN(">", OP, GT),
    KEYWORD_PATTERN(">=", OP, GE),
    KEYWORD_PATTERN("!=", OP, NE),
    NAME_KEYWORD_PATTERN("?", "\\?", OP, COND), // 40

    NAME_KEYWORD_PATTERN("*=", "\\*=", OP, MUL_ASSN),
    KEYWORD_PATTERN("/=", OP, DIV_ASSN),
    KEYWORD_PATTERN("%=", OP, MOD_ASSN),
    NAME_KEYWORD_PATTERN("+=", "\\+=", OP, ADD_ASSN),
    KEYWORD_PATTERN("-=", OP, SUB_ASSN),
    KEYWORD_PATTERN("<<=", OP, LEFT_ASSN),
    KEYWORD_PATTERN(">>=", OP, RIGHT_ASSN),
    KEYWORD_PATTERN("&=", OP, AND_ASSN),
    KEYWORD_PATTERN("^=", OP, XOR_ASSN),
    NAME_KEYWORD_PATTERN("|=", "\\|=", OP, OR_ASSN),

    NAME_KEYWORD_PATTERN("++", "\\+\\+", OP, INC),
    KEYWORD_PATTERN("--", OP, DEC),
};

struct token_pattern g_nonterms[MAX_NONTERMS];
u16 g_nonterm_count;

struct hashtable token_patterns_by_symbol;

const char *token_type_strings[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)
};

void token_init()
{
    hashtable_init(&token_patterns_by_symbol);
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &token_patterns[i];
        if(tp->name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->name){
            tp->symbol_name = to_symbol(tp->name);
            hashtable_set_p(&token_patterns_by_symbol, tp->symbol_name, tp);
        }
    }
    g_nonterm_count = 0;
}

void token_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &token_patterns[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
    hashtable_deinit(&token_patterns_by_symbol);
}

struct token_patterns get_token_patterns()
{
    struct token_patterns tps = { token_patterns, TERMINAL_COUNT };
    return tps;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < OP_TOTAL);
    return get_token_pattern_by_opcode(opcode)->name;
}

symbol get_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode)
{
    if (token_type == TOKEN_OP){
        assert(opcode >= 0 && opcode < OP_TOTAL);
        return get_token_pattern_by_opcode(opcode)->symbol_name;
    }else{
        return get_token_pattern_by_token_type(token_type)->symbol_name;
    }
}

struct token_pattern *get_token_pattern_by_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < TERMINAL_COUNT);
    return &token_patterns[(int)TOKEN_OP + (int)opcode];
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &token_patterns[(int)token_type];
}

struct token_pattern *get_token_pattern_by_symbol(symbol symbol)
{
    return (struct token_pattern *)hashtable_get_p(&token_patterns_by_symbol, symbol);
}

//the symbol could be terminal or non-terminal symbol in grammar
u16 get_symbol_index(symbol symbol)
{
    struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
    if(tp == 0){
        printf("unkown symbol: %s\n", string_get(symbol));
        assert(false);
    }
    
    if(tp->token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)tp->opcode;
    return (u16)tp->token_type;
}

u16 get_token_index(enum token_type token_type, enum op_code opcode)
{
    symbol symbol = get_symbol_by_token_opcode(token_type, opcode);
    return get_symbol_index(symbol);
}

u16 register_grammar_nonterm(symbol symbol)
{
    struct token_pattern *tp = get_token_pattern_by_symbol(symbol);
    if(tp){
        printf("already found the grammar nonterm symbol: %s\n", string_get(symbol));
        assert(false);
    }
    //add nonterm grammar symbol, TODO: need to check existence of symbol, don't add if already existed
    struct token_pattern tpn;
    u16 nonterm = (u16)g_nonterm_count + (u16)TERMINAL_COUNT;
    tpn.token_type = nonterm;
    tpn.symbol_name = symbol;
    tpn.name = string_get(tpn.symbol_name);
    tpn.opcode = 0;
    tpn.pattern = 0;
    tpn.re = 0; 
    g_nonterms[g_nonterm_count] = tpn;
    hashtable_set_p(&token_patterns_by_symbol, tpn.symbol_name, &g_nonterms[g_nonterm_count]);
    g_nonterm_count++;  
    return nonterm;
}

u16 get_symbol_count()
{
    return g_nonterm_count + (u16)TERMINAL_COUNT;
}

bool is_terminal(u16 symbol_index)
{
    return symbol_index < TERMINAL_COUNT;
}

symbol get_symbol_by_index(u16 symbol_index)
{
    struct token_pattern *tp;
    if (symbol_index < TERMINAL_COUNT){
        tp = &token_patterns[symbol_index];
    }else{
        symbol_index -= TERMINAL_COUNT;
        assert(symbol_index < g_nonterm_count);
        tp = &g_nonterms[symbol_index];
    }
    return tp->symbol_name;
}
