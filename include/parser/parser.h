/*
 * parser.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include "clib/hashtable.h"
#include "clib/queue.h"
#include "clib/symboltable.h"
#include "lexer/lexer.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

struct parser {
    /* mapping type string into type enum: hashtable of (symbol, int) */
    struct hashtable symbol_2_int_types;

    /* hashtable of (symbol, exp_node*) for ext types */
    struct hashtable ext_types;

    /* hashtable of (symbol, int of precdence level*/
    struct hashtable op_precs;

    struct symboltable vars; /*symbol table of */

    symbol type_of; //type-of symbol, the syntax is :
    symbol assignment; //assignment symbol: =
    symbol comma;
    symbol binary;
    symbol unary;
    symbol import;
    symbol extern_symbol;
    symbol type;
    symbol variadic;
    symbol lparen;
    symbol rparen;
    symbol lbracket;
    symbol rbracket;
    symbol if_symbol;
    symbol else_symbol;
    symbol then_symbol;
    symbol for_symbol;
    symbol in_symbol;
    symbol range_symbol;
    symbol true_symbol;
    symbol false_symbol;

    symbol plus_op;
    symbol minus_op;
    symbol multiply_op;
    symbol division_op;
    symbol modulo_op;
    symbol lessthan_op;
    symbol greaterthan_op;
    symbol lessthanequal_op;
    symbol greaterthanequal_op;
    symbol equal_op;
    symbol notequal_op;

    symbol or_op;
    symbol and_op;
    symbol not_op;

    struct token curr_token;
    struct ast *ast;
    bool allow_id_as_a_func;
    bool id_is_var_decl;
    bool is_repl;
    struct module *current_module;
    struct queue queued_tokens; //queue of token
};

typedef void (*exp_executor)(void *, struct exp_node *);

struct parser *parser_new(bool is_repl);
void parser_free(struct parser *parser);
void parse_next_token(struct parser *parser);
struct exp_node *parse_exp_to_function(struct parser *parser, struct exp_node *exp, symbol fn);
struct exp_node *parse_import(struct parser *parser, struct exp_node *parent);
struct exp_node *parse_statement(struct parser *parser, struct exp_node *parent);
struct exp_node *parse_exp(struct parser *parser, struct exp_node *parent, struct exp_node *lhs);
bool is_unary_op(struct prototype_node *pnode);
bool is_binary_op(struct prototype_node *pnode);
char get_op_name(struct prototype_node *pnode);
void queue_token(struct parser *parser, struct token tkn);
void queue_tokens(struct parser *psr, struct array *tokens);
struct block_node *parse_file(struct parser *parser, const char *file_name);
struct block_node *parse_file_object(struct parser *parser, const char *mod_name, FILE *file);
struct block_node *parse_repl(struct parser *parser, void (*fun)(void *, struct exp_node *), void *jit);
struct block_node *parse_string(struct parser *parser, const char *mod_name, const char *code);
enum type get_type_enum(symbol type_symbol);
symbol get_type_symbol(enum type type_enum);

#ifdef __cplusplus
}
#endif

#endif
