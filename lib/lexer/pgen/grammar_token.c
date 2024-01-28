/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * token.c implements common terminals required in m. we actually can generate the terminal list from grammar.
 */
#include "lexer/pgen/grammar_token.h"
#include "clib/hashtable.h"
#include <assert.h>

#define TOKEN_PATTERN(tok_name, pattern, class_name) {#tok_name, pattern, TOKEN_##tok_name, 0, class_name, 0, 0}

#define KEYWORD_PATTERN(tok_name, keyword) {keyword, keyword, TOKEN_##tok_name, 0, "keyword", 0, 0}
#define TOKEN(tok_name, name, pattern, class_name) {name, pattern, TOKEN_##tok_name, 0, class_name, 0, 0}

#define OP(op_name, name, pattern) {name, pattern, TOKEN_OP, OP_##op_name, "operator", 0, 0}

struct token_pattern _token_patterns[TERMINAL_COUNT] = {
    TOKEN_PATTERN(NULL, 0, 0),    // 1
    TOKEN_PATTERN(EOF, 0, 0),     // 1
    TOKEN_PATTERN(EPSILON, 0, 0),

    TOKEN_PATTERN(INDENT, 0, 0),
    TOKEN_PATTERN(DEDENT, 0, 0),
    TOKEN_PATTERN(LITERAL_CHAR, 0, "string"),
    TOKEN_PATTERN(LITERAL_STRING, 0, "string"),
    TOKEN_PATTERN(NEWLINE, "\n", 0),
    TOKEN_PATTERN(LITERAL_INT, "[0-9]+|0x[0-9a-fA-F]+", "number"),
    TOKEN_PATTERN(LITERAL_FLOAT, "([0-9]*.)?[0-9]+", "number"),
    TOKEN_PATTERN(LITERAL_COMPLEX, "([0-9]*.)?[0-9]+ \\+ ([0-9]*.)?[0-9]*i", "number"),

    TOKEN(PYCOMMENT, "#", "#", "pycomment"),
    TOKEN(LINECOMMENT, "//", "//", "comment"),
    TOKEN(BLOCKCOMMENT, "/*", "/\\*", "block-comment"),
    TOKEN(WILDCARD, "_", "_", "variable"), 

    KEYWORD_PATTERN(FROM, "from"), 
    KEYWORD_PATTERN(MEMORY, "memory"),
    KEYWORD_PATTERN(EXTERN, "extern"), 

    //WIT items
    KEYWORD_PATTERN(NONE, "None"), 
    KEYWORD_PATTERN(TYPE_BOOL, "bool"), 
    KEYWORD_PATTERN(TYPE_INT, "int"), 
    KEYWORD_PATTERN(TYPE_U8, "u8"), 
    KEYWORD_PATTERN(TYPE_U16, "u16"), 
    KEYWORD_PATTERN(TYPE_U32, "u32"), 
    KEYWORD_PATTERN(TYPE_U64, "u64"), 
    KEYWORD_PATTERN(TYPE_I8, "i8"), 
    KEYWORD_PATTERN(TYPE_I16, "i16"), 
    KEYWORD_PATTERN(TYPE_I32, "i32"), 
    KEYWORD_PATTERN(TYPE_I64, "i64"), 
    KEYWORD_PATTERN(TYPE_F32, "f32"), 
    KEYWORD_PATTERN(TYPE_F64, "f64"), 
    KEYWORD_PATTERN(TYPE_CHAR, "char"), 
    KEYWORD_PATTERN(TYPE_STRING, "string"), 
    
    KEYWORD_PATTERN(USE, "use"), 
    KEYWORD_PATTERN(TYPE, "type"),
    KEYWORD_PATTERN(RESOURCE, "resource"), 
    KEYWORD_PATTERN(FUN, "func"),
    KEYWORD_PATTERN(STRUCT, "struct"),
    KEYWORD_PATTERN(RECORD, "record"),
    KEYWORD_PATTERN(ENUM, "enum"),
    KEYWORD_PATTERN(FLAGS, "flags"),
    KEYWORD_PATTERN(VARIANT, "variant"),
    KEYWORD_PATTERN(UNION, "union"),
    KEYWORD_PATTERN(OPTION, "option"),
    KEYWORD_PATTERN(LIST, "list"),
    KEYWORD_PATTERN(RESULT, "result"),
    KEYWORD_PATTERN(AS, "as"),
    KEYWORD_PATTERN(STATIC, "static"),
    KEYWORD_PATTERN(INTERFACE, "interface"),
    KEYWORD_PATTERN(WORLD, "world"),
    KEYWORD_PATTERN(TUPLE, "tuple"),
    KEYWORD_PATTERN(FUTURE, "future"),
    KEYWORD_PATTERN(STREAM, "stream"),
    KEYWORD_PATTERN(IMPORT, "import"),
    KEYWORD_PATTERN(EXPORT, "export"),
    KEYWORD_PATTERN(PACKAGE, "package"),
    KEYWORD_PATTERN(INCLUDE, "include"),
    KEYWORD_PATTERN(CLASS, "class"),
    KEYWORD_PATTERN(BORROW, "borrow"),
    KEYWORD_PATTERN(DEFAULT, "default"),

    KEYWORD_PATTERN(DEF, "def"),
    KEYWORD_PATTERN(LET, "let"),
    KEYWORD_PATTERN(MUT, "mut"),
    KEYWORD_PATTERN(MAPTO, "->"),
    KEYWORD_PATTERN(IF, "if"),
    KEYWORD_PATTERN(ELIF, "elif"),
    KEYWORD_PATTERN(ELSE, "else"), 

    KEYWORD_PATTERN(TRUE, "True"),
    KEYWORD_PATTERN(FALSE, "False"),
    KEYWORD_PATTERN(IN, "in"),
    KEYWORD_PATTERN(FOR, "for"),
    KEYWORD_PATTERN(WHILE, "while"),
    KEYWORD_PATTERN(BREAK, "break"),
    KEYWORD_PATTERN(CONTINUE, "continue"),
    KEYWORD_PATTERN(RETURN, "return"),
    KEYWORD_PATTERN(CONSTRUCTOR, "constructor"),

    KEYWORD_PATTERN(MATCH, "match"),
    KEYWORD_PATTERN(WITH, "with"),
    KEYWORD_PATTERN(WHEN, "when"),
    
    KEYWORD_PATTERN(NEW, "new"),
    KEYWORD_PATTERN(DEL, "del"),

    /*reserved keywords*/
    KEYWORD_PATTERN(YIELD, "yield"),
    KEYWORD_PATTERN(ASYNC, "async"),
    KEYWORD_PATTERN(AWAIT, "await"),

    TOKEN_PATTERN(IDENT, "[_a-zA-Z][_a-zA-Z0-9]*", 0), 

    //operators
    TOKEN(LPAREN, "(", "\\(", "keyword"),
    TOKEN(RPAREN, ")", "\\)", "keyword"),
    TOKEN(LBRACKET, "[", "\\[", "keyword"),
    TOKEN(RBRACKET, "]", "\\]", "keyword"),
    KEYWORD_PATTERN(LCBRACKET, "{"), // 25
    KEYWORD_PATTERN(RCBRACKET, "}"),

    KEYWORD_PATTERN(COMMA, ","),
    KEYWORD_PATTERN(SEMICOLON, ";"),

    TOKEN(RANGE, "..", "\\.\\.", "keyword"),
    TOKEN(VARIADIC, "...", "\\.\\.\\.", "keyword"),
    KEYWORD_PATTERN(ISTYPEOF, ":"),
    
    /*operator separator*/
    TOKEN_PATTERN(OP, 0, "operator"),

    OP( DOT, ".", "\\."), // literal dot
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
