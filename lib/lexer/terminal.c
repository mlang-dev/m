/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * terminal.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/terminal.h"
#include "clib/hashtable.h"
#include <assert.h>

#define NULL_PATTERN(pattern, tok_name, op_name, class_name) {0, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name, class_name}
#define TOKEN_PATTERN(pattern, tok_name, op_name, class_name) {#tok_name, 0, pattern, 0, TOKEN_##tok_name, OP_##op_name, class_name}
#define KEYWORD_PATTERN(keyword, tok_name, op_name, class_name) {keyword, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name, class_name}
#define NAME_KEYWORD_PATTERN(name, keyword, tok_name, op_name, class_name) {name, 0, keyword, 0, TOKEN_##tok_name, OP_##op_name, class_name}

struct token_pattern g_token_patterns[TERMINAL_COUNT] = {
    TOKEN_PATTERN(0, NULL, NULL, 0), // 1
    TOKEN_PATTERN(0, EOF, NULL, 0), // 1
    TOKEN_PATTERN(0, EPSILON, NULL, 0),
    TOKEN_PATTERN(0, INDENT, NULL, 0),
    TOKEN_PATTERN(0, DEDENT, NULL, 0),
    KEYWORD_PATTERN("//", LINECOMMENT, NULL, "comment"),
    NAME_KEYWORD_PATTERN("/*", "/\\*", BLOCKCOMMENT, NULL, "block-comment"),

    KEYWORD_PATTERN("_", WILDCARD, NULL, "variable"), // 10

    TOKEN_PATTERN("\n", NEWLINE, NULL, 0),
    TOKEN_PATTERN("[0-9]+|0x[0-9a-fA-F]+", LITERAL_INT, NULL, "number"), // 5
    TOKEN_PATTERN("([0-9]*.)?[0-9]+", LITERAL_FLOAT, NULL, "number"),
    TOKEN_PATTERN("([0-9]*.)?[0-9]+ \\+ ([0-9]*.)?[0-9]*i", LITERAL_COMPLEX, NULL, "number"),
    TOKEN_PATTERN(0, LITERAL_CHAR, NULL, "string"),
    TOKEN_PATTERN(0, LITERAL_STRING, NULL, "string"),

    KEYWORD_PATTERN("from", FROM, NULL, "keyword"), 
    KEYWORD_PATTERN("memory", MEMORY, NULL, "keyword"),
    KEYWORD_PATTERN("extern", EXTERN, NULL, "keyword"), 

    //WIT items
    NAME_KEYWORD_PATTERN("()", "\\(\\)", TYPE_UNIT, NULL, "keyword"),
    KEYWORD_PATTERN("bool", TYPE_BOOL, NULL, "keyword"), 
    KEYWORD_PATTERN("int", TYPE_INT, NULL, "keyword"), 
    KEYWORD_PATTERN("u8", TYPE_U8, NULL, "keyword"), 
    KEYWORD_PATTERN("u16", TYPE_U16, NULL, "keyword"), 
    KEYWORD_PATTERN("u32", TYPE_U32, NULL, "keyword"), 
    KEYWORD_PATTERN("u64", TYPE_U64, NULL, "keyword"), 
    KEYWORD_PATTERN("i8", TYPE_I8, NULL, "keyword"), 
    KEYWORD_PATTERN("i16", TYPE_I16, NULL, "keyword"), 
    KEYWORD_PATTERN("i32", TYPE_I32, NULL, "keyword"), 
    KEYWORD_PATTERN("i64", TYPE_I64, NULL, "keyword"), 
    KEYWORD_PATTERN("f32", TYPE_F32, NULL, "keyword"), 
    KEYWORD_PATTERN("f64", TYPE_F64, NULL, "keyword"), 
    KEYWORD_PATTERN("char", TYPE_CHAR, NULL, "keyword"), 
    KEYWORD_PATTERN("string", TYPE_STRING, NULL, "keyword"), 
    
    KEYWORD_PATTERN("use", USE, NULL, "keyword"), 
    KEYWORD_PATTERN("type", TYPE, NULL, "keyword"),
    KEYWORD_PATTERN("resource", RESOURCE, NULL, "keyword"), 
    KEYWORD_PATTERN("fun", FUN, NULL, "keyword"),
    KEYWORD_PATTERN("struct", STRUCT, NULL, "keyword"),
    KEYWORD_PATTERN("enum", ENUM, NULL, "keyword"),
    KEYWORD_PATTERN("flags", FLAGS, NULL, "keyword"),
    KEYWORD_PATTERN("variant", VARIANT, NULL, "keyword"),
    KEYWORD_PATTERN("union", UNION, NULL, "keyword"),
    KEYWORD_PATTERN("option", OPTION, NULL, "keyword"),
    KEYWORD_PATTERN("list", LIST, NULL, "keyword"),
    KEYWORD_PATTERN("result", RESULT, NULL, "keyword"),
    KEYWORD_PATTERN("as", AS, NULL, "keyword"),
    KEYWORD_PATTERN("static", STATIC, NULL, "keyword"),
    KEYWORD_PATTERN("interface", INTERFACE, NULL, "keyword"),
    KEYWORD_PATTERN("world", WORLD, NULL, "keyword"),
    KEYWORD_PATTERN("tuple", TUPLE, NULL, "keyword"),
    KEYWORD_PATTERN("future", FUTURE, NULL, "keyword"),
    KEYWORD_PATTERN("stream", STREAM, NULL, "keyword"),
    KEYWORD_PATTERN("import", IMPORT, NULL, "keyword"),
    KEYWORD_PATTERN("export", EXPORT, NULL, "keyword"),
    KEYWORD_PATTERN("default", DEFAULT, NULL, "keyword"),

    KEYWORD_PATTERN("let", LET, NULL, "keyword"),
    KEYWORD_PATTERN("mut", MUT, NULL, "keyword"),
    KEYWORD_PATTERN("->", MAPTO, NULL, "keyword"),
    KEYWORD_PATTERN("if", IF, NULL, "keyword"),
    KEYWORD_PATTERN("then", THEN, NULL, "keyword"),
    KEYWORD_PATTERN("else", ELSE, NULL, "keyword"), // 15

    KEYWORD_PATTERN("true", TRUE, NULL, "keyword"),
    KEYWORD_PATTERN("false", FALSE, NULL, "keyword"),
    KEYWORD_PATTERN("in", IN, NULL, "keyword"),
    KEYWORD_PATTERN("for", FOR, NULL, "keyword"),
    KEYWORD_PATTERN("while", WHILE, NULL, "keyword"),
    KEYWORD_PATTERN("break", BREAK, NULL, "keyword"),
    KEYWORD_PATTERN("continue", CONTINUE, NULL, "keyword"),
    KEYWORD_PATTERN("return", RETURN, NULL, "keyword"),


    NAME_KEYWORD_PATTERN("(", "\\(", LPAREN, NULL, "operator"),
    NAME_KEYWORD_PATTERN(")", "\\)", RPAREN, NULL, "operator"),
    NAME_KEYWORD_PATTERN("[", "\\[", LBRACKET, NULL, "operator"),
    NAME_KEYWORD_PATTERN("]", "\\]", RBRACKET, NULL, "operator"),
    KEYWORD_PATTERN("{", LCBRACKET, NULL, "operator"), // 25
    KEYWORD_PATTERN("}", RCBRACKET, NULL, "operator"),

    KEYWORD_PATTERN(",", COMMA, NULL, "operator"),
    KEYWORD_PATTERN(";", SEMICOLON, NULL, "operator"),


    NAME_KEYWORD_PATTERN("..", "\\.\\.", RANGE, NULL, "operator"),
    NAME_KEYWORD_PATTERN("...", "\\.\\.\\.", VARIADIC, NULL, "operator"), // 30
    KEYWORD_PATTERN(":", ISTYPEOF, NULL, "operator"),

    KEYWORD_PATTERN("match", MATCH, NULL, "keyword"),
    KEYWORD_PATTERN("with", WITH, NULL, "keyword"),
    KEYWORD_PATTERN("when", WHEN, NULL, "keyword"),
    
    KEYWORD_PATTERN("new", NEW, NULL, "keyword"),
    KEYWORD_PATTERN("del", DEL, NULL, "keyword"),

    /*reserved keywords*/
    KEYWORD_PATTERN("yield", YIELD, NULL, "keyword"),
    KEYWORD_PATTERN("async", ASYNC, NULL, "keyword"),
    KEYWORD_PATTERN("await", AWAIT, NULL, "keyword"),

    TOKEN_PATTERN("[_a-zA-Z][_a-zA-Z0-9]*", IDENT, NULL, 0), //
    
    /*operators*/
    TOKEN_PATTERN(0, OP, NULL, "operator"),
    NAME_KEYWORD_PATTERN(".", "\\.", OP, DOT, "operator"), // literal dot
    NAME_KEYWORD_PATTERN("||", "\\|\\|", OP, OR, "operator"),
    KEYWORD_PATTERN("&&", OP, AND, "operator"), // 35
    KEYWORD_PATTERN("!", OP, NOT, "operator"),

    KEYWORD_PATTERN("~", OP, BITNOT, "operator"),
    NAME_KEYWORD_PATTERN("|", "\\|", OP, BITOR, "operator"),
    KEYWORD_PATTERN("^", OP, BITEXOR, "operator"),
    KEYWORD_PATTERN("&", OP, BAND, "operator"), //or reference
    KEYWORD_PATTERN("<<", OP, BSL, "operator"),
    KEYWORD_PATTERN(">>", OP, BSR, "operator"),

    // KEYWORD_PATTERN("^", OP, EXPO),
    NAME_KEYWORD_PATTERN("|/", "\\|/", OP, SQRT, "operator"),
    NAME_KEYWORD_PATTERN("**", "\\*\\*", OP, POW, "operator"), // 40
    NAME_KEYWORD_PATTERN("*", "\\*", OP, STAR, "operator"), // 40
    KEYWORD_PATTERN("/", OP, DIVISION, "operator"),
    KEYWORD_PATTERN("%", OP, MODULUS, "operator"),
    NAME_KEYWORD_PATTERN("+", "\\+", OP, PLUS, "operator"),
    KEYWORD_PATTERN("-", OP, MINUS, "operator"),

    KEYWORD_PATTERN("<", OP, LT, "operator"), // 45
    KEYWORD_PATTERN("<=", OP, LE, "operator"),
    KEYWORD_PATTERN("==", OP, EQ, "operator"),
    KEYWORD_PATTERN(">", OP, GT, "operator"),
    KEYWORD_PATTERN(">=", OP, GE, "operator"),
    KEYWORD_PATTERN("!=", OP, NE, "operator"),
    NAME_KEYWORD_PATTERN("?", "\\?", OP, COND, "operator"), // 40

    KEYWORD_PATTERN("=", OP, ASSIGN, "operator"),
    NAME_KEYWORD_PATTERN("*=", "\\*=", OP, MUL_ASSN, "operator"),
    KEYWORD_PATTERN("/=", OP, DIV_ASSN, "operator"),
    KEYWORD_PATTERN("%=", OP, MOD_ASSN, "operator"),
    NAME_KEYWORD_PATTERN("+=", "\\+=", OP, ADD_ASSN, "operator"),
    KEYWORD_PATTERN("-=", OP, SUB_ASSN, "operator"),
    KEYWORD_PATTERN("<<=", OP, LEFT_ASSN, "operator"),
    KEYWORD_PATTERN(">>=", OP, RIGHT_ASSN, "operator"),
    KEYWORD_PATTERN("&=", OP, AND_ASSN, "operator"),
    KEYWORD_PATTERN("^=", OP, XOR_ASSN, "operator"),
    NAME_KEYWORD_PATTERN("|=", "\\|=", OP, OR_ASSN, "operator"),

    NAME_KEYWORD_PATTERN("++", "\\+\\+", OP, INC, "operator"),
    KEYWORD_PATTERN("--", OP, DEC, "operator"),
};


void terminal_init()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->name){
            tp->symbol_name = to_symbol(tp->name);
        }
    }
}

void terminal_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &g_token_patterns[i];
        if(tp->re){
            regex_free(tp->re);
            tp->re = 0;
        }
    }
}

void tok_clean(struct token *tok)
{
    if(tok->token_type == TOKEN_LITERAL_STRING && tok->str_val){
        FREE((void*)tok->str_val);
        tok->str_val = 0;
    }
}

struct token_patterns get_token_patterns()
{
    struct token_patterns tps = { g_token_patterns, TERMINAL_COUNT };
    return tps;
}

const char *get_opcode(enum op_code opcode)
{
    assert(opcode > 0 && opcode < OP_TOTAL);
    return get_token_pattern_by_opcode(opcode)->name;
}

symbol get_terminal_symbol_by_token_opcode(enum token_type token_type, enum op_code opcode)
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
    return &g_token_patterns[(int)TOKEN_OP + (int)opcode];
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &g_token_patterns[(int)token_type];
}

u16 get_terminal_token_index(enum token_type token_type, enum op_code opcode)
{
    if(token_type == TOKEN_OP)
        return (u16)TOKEN_OP + (u16)opcode;
    else 
        return (u16)token_type;
}

bool is_terminal(u16 symbol_index)
{
    return symbol_index < TERMINAL_COUNT;
}

bool is_relational_op(enum op_code opcode)
{
    return opcode >= OP_LT && opcode <= OP_NE;
}

enum op_code get_op_code_from_assign_op(enum op_code assign_op)
{
    switch (assign_op)
    {
    case OP_MUL_ASSN:
        return OP_STAR;
    case OP_DIV_ASSN:
        return OP_DIVISION;
    case OP_MOD_ASSN:
        return OP_MODULUS;
    case OP_ADD_ASSN:
        return OP_PLUS;
    case OP_SUB_ASSN:
        return OP_MINUS;
    case OP_LEFT_ASSN:
        return OP_BSL;
    case OP_RIGHT_ASSN:
        return OP_RIGHT_ASSN;
    case OP_AND_ASSN:
        return OP_BAND;
    case OP_XOR_ASSN:
        return OP_BITEXOR;
    case OP_OR_ASSN:
        return OP_BITOR;
    default:
        assert(false);
        return OP_NULL;
    }
}

