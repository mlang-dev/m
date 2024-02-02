/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * token.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/pgen/grammar_token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define TOKEN(tok_name, name, pattern, class_name) {name, pattern, TOKEN_##tok_name, 0, class_name, 0, 0}
#define OP(op_name, name, pattern) {name, pattern, TOKEN_OP, OP_##op_name, "operator", 0, 0}

struct token_pattern _token_patterns[TERMINAL_COUNT] = {
    TOKEN(NULL, "NULL",  0, 0),    // 1
    TOKEN(EOF, "EOF",  0, 0),     // 1
    TOKEN(EPSILON, "EPSILON", 0, 0),

    TOKEN(INDENT, "INDENT", 0, 0),
    TOKEN(DEDENT, "DEDENT", 0, 0),
    TOKEN(LITERAL_CHAR, "LITERAL_CHAR", 0, "string"),
    TOKEN(LITERAL_STRING, "LITERAL_STRING", 0, "string"),
    TOKEN(NEWLINE, "NEWLINE", "\n", 0),
    TOKEN(LITERAL_INT, "LITERAL_INT", "[0-9]+|0x[0-9a-fA-F]+", "number"),
    TOKEN(LITERAL_FLOAT, "LITERAL_FLOAT", "([0-9]*.)?[0-9]+", "number"),
    TOKEN(LITERAL_COMPLEX, "LITERAL_COMPLEX", "([0-9]*.)?[0-9]+ \\+ ([0-9]*.)?[0-9]*i", "number"),

    TOKEN(PYCOMMENT, "#", "#", "pycomment"),
    TOKEN(LINECOMMENT, "//", "//", "comment"),
    TOKEN(BLOCKCOMMENT, "/*", "/\\*", "block-comment"),
    TOKEN(WILDCARD, "_", "_", "variable"), 

    TOKEN(FROM, "from", "from", "keyword"), 
    TOKEN(MEMORY, "memory", "memory", "keyword"),
    TOKEN(EXTERN, "extern", "extern", "keyword"), 

    //WIT items
    TOKEN(NONE, "None", "None", "keyword"), 
    TOKEN(TYPE_BOOL, "bool", "bool", "keyword"), 
    TOKEN(TYPE_INT, "int", "int", "keyword"), 
    TOKEN(TYPE_U8, "u8", "u8", "keyword"), 
    TOKEN(TYPE_U16, "u16", "u16", "keyword"), 
    TOKEN(TYPE_U32, "u32", "u32", "keyword"), 
    TOKEN(TYPE_U64, "u64", "u64", "keyword"), 
    TOKEN(TYPE_I8, "i8", "i8", "keyword"), 
    TOKEN(TYPE_I16, "i16", "i16", "keyword"), 
    TOKEN(TYPE_I32, "i32", "i32", "keyword"), 
    TOKEN(TYPE_I64, "i64", "i64", "keyword"), 
    TOKEN(TYPE_F32, "f32", "f32", "keyword"), 
    TOKEN(TYPE_F64, "f64", "f64", "keyword"), 
    TOKEN(TYPE_CHAR, "char", "char", "keyword"), 
    TOKEN(TYPE_STRING, "string", "string", "keyword"), 
    
    TOKEN(USE, "use", "use", "keyword"), 
    TOKEN(TYPE, "type", "type", "keyword"),
    TOKEN(RESOURCE, "resource", "resource", "keyword"), 
    TOKEN(FUN, "func", "func", "keyword"),
    TOKEN(STRUCT, "struct", "struct", "keyword"),
    TOKEN(RECORD, "record", "record", "keyword"),
    TOKEN(ENUM, "enum", "enum", "keyword"),
    TOKEN(FLAGS, "flags", "flags", "keyword"),
    TOKEN(VARIANT, "variant", "variant", "keyword"),
    TOKEN(UNION, "union", "union", "keyword"),
    TOKEN(OPTION, "option", "option", "keyword"),
    TOKEN(LIST, "list", "list", "keyword"),
    TOKEN(RESULT, "result", "result", "keyword"),
    TOKEN(AS, "as", "as", "keyword"),
    TOKEN(STATIC, "static", "static", "keyword"),
    TOKEN(INTERFACE, "interface", "interface", "keyword"),
    TOKEN(WORLD, "world", "world", "keyword"),
    TOKEN(TUPLE, "tuple", "tuple", "keyword"),
    TOKEN(FUTURE, "future", "future", "keyword"),
    TOKEN(STREAM, "stream", "stream", "keyword"),
    TOKEN(IMPORT, "import", "import", "keyword"),
    TOKEN(EXPORT, "export", "export", "keyword"),
    TOKEN(PACKAGE, "package", "package", "keyword"),
    TOKEN(INCLUDE, "include", "include", "keyword"),
    TOKEN(CLASS, "class", "class", "keyword"),
    TOKEN(BORROW, "borrow", "borrow", "keyword"),
    TOKEN(DEFAULT, "default", "default", "keyword"),

    TOKEN(DEF, "def", "def", "keyword"),
    TOKEN(LET, "let", "let", "keyword"),
    TOKEN(MUT, "mut", "mut", "keyword"),
    TOKEN(MAPTO, "->", "->", "keyword"),
    TOKEN(IF, "if", "if", "keyword"),
    TOKEN(ELIF, "elif", "elif", "keyword"),
    TOKEN(ELSE, "else", "else", "keyword"), 

    TOKEN(TRUE, "True", "True", "keyword"),
    TOKEN(FALSE, "False", "False", "keyword"),
    TOKEN(IN, "in", "in", "keyword"),
    TOKEN(FOR, "for", "for", "keyword"),
    TOKEN(WHILE, "while", "while", "keyword"),
    TOKEN(BREAK, "break", "break", "keyword"),
    TOKEN(CONTINUE, "continue", "continue", "keyword"),
    TOKEN(RETURN, "return", "return", "keyword"),
    TOKEN(CONSTRUCTOR, "constructor", "constructor", "keyword"),

    TOKEN(MATCH, "match", "match", "keyword"),
    TOKEN(WITH, "with", "with", "keyword"),
    TOKEN(WHEN, "when", "when", "keyword"),
    
    TOKEN(NEW, "new", "new", "keyword"),
    TOKEN(DEL, "del", "del", "keyword"),

    /*reserved keywords*/
    TOKEN(YIELD, "yield", "yield", "keyword"),
    TOKEN(ASYNC, "async", "async", "keyword"),
    TOKEN(AWAIT, "await", "await", "keyword"),

    TOKEN(IDENT, "IDENT", "[_a-zA-Z][_a-zA-Z0-9]*", 0), 

    //operators
    TOKEN(LPAREN, "(", "\\(", "keyword"),
    TOKEN(RPAREN, ")", "\\)", "keyword"),
    TOKEN(LBRACKET, "[", "\\[", "keyword"),
    TOKEN(RBRACKET, "]", "\\]", "keyword"),
    TOKEN(LCBRACKET, "{", "{", "keyword"), // 25
    TOKEN(RCBRACKET, "}", "}", "keyword"),

    TOKEN(COMMA, ",", ",", "keyword"),
    TOKEN(SEMICOLON, ";", ";", "keyword"),

    TOKEN(RANGE, "..", "\\.\\.", "keyword"),
    TOKEN(VARIADIC, "...", "\\.\\.\\.", "keyword"),
    TOKEN(ISTYPEOF, ":", ":", "keyword"),

    /*operator separator*/
    TOKEN(OP, "OP", 0, "operator"),
    
    OP(DOT, ".", "\\."), // literal dot
    OP(OR, "or", "or"),
    OP(AND, "and", "and"), // 35
    OP(NOT, "not", "not"),

    OP(BITNOT, "~", "~"),
    OP(BITOR, "|", "\\|"),
    OP(BITEXOR, "^", "^"),
    OP(BAND, "&", "&"), //or reference
    OP(BSL, "<<", "<<"),
    OP(BSR, ">>", ">>"),

    // KEYWORD_PATTERN("^", EXPO),
    OP(SQRT, "|/", "\\|/"),
    OP(POW, "**", "\\*\\*"), // 40
    OP(STAR, "*", "\\*"), // 40
    OP(DIVISION, "/", "/"),
    OP(MODULUS, "%", "%"),
    OP(PLUS, "+", "\\+"),
    OP(MINUS, "-", "-"),

    OP(LT, "<", "<"), // 45
    OP(LE, "<=", "<="),
    OP(EQ, "==", "=="),
    OP(GT, ">", ">"),
    OP(GE, ">=", ">="),
    OP(NE, "!=", "!="),
    OP(COND, "?", "\\?"), // 40

    OP(ASSIGN, "=", "="),
    OP(MUL_ASSN, "*=", "\\*="),
    OP(DIV_ASSN, "/=", "/="),
    OP(MOD_ASSN, "%=", "%="),
    OP(ADD_ASSN, "+=", "\\+="),
    OP(SUB_ASSN, "-=", "-="),
    OP(LEFT_ASSN, "<<=", "<<="),
    OP(RIGHT_ASSN, ">>=", ">>="),
    OP(AND_ASSN, "&=", "&="),
    OP(XOR_ASSN, "^=", "^="),
    OP(OR_ASSN, "|=", "\\|="),

    OP(INC, "++", "\\+\\+"),
    OP(DEC, "--", "--"),
    OP(AT, "@", "@"),
};

void token_init()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &_token_patterns[i];
        if(tp->token_name&&tp->pattern&&!tp->re){
            tp->re = regex_new(tp->pattern);
            assert(tp->re);
        }
        if(tp->token_name){
            tp->symbol_name = to_symbol(tp->token_name);
        }
    }
}

void token_deinit()
{
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        struct token_pattern *tp = &_token_patterns[i];
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
    struct token_patterns tps = { _token_patterns, TERMINAL_COUNT };
    return tps;
}

struct token_pattern *get_token_pattern_by_token_type(enum token_type token_type)
{
    assert(token_type >= 0 && token_type <= TOKEN_OP);
    return &_token_patterns[(int)token_type];
}
