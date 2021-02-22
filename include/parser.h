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

#include "ast.h"
#include "lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct parser {
    struct hashtable types; /*hashtable of (string, int)*/
    struct hashtable ext_types; /*hashtable of (string, exp_node*) for ext types*/
    struct hashtable op_precs;
    struct token curr_token;
    struct ast* ast;
    bool allow_id_as_a_func;
    bool id_is_var_decl;
    bool is_repl;
    struct module* current_module;
    struct queue queued_tokens; //queue of token
};

typedef void (*exp_executor)(void*, struct exp_node*);

struct parser* parser_new(bool is_repl);
void parser_free(struct parser* parser);
void parse_next_token(struct parser* parser);
struct exp_node* parse_exp_to_function(struct parser* parser, struct exp_node* exp, const char* fn);
struct exp_node* parse_import(struct parser* parser, struct exp_node* parent);
struct exp_node* parse_statement(struct parser* parser, struct exp_node* parent);
struct exp_node* parse_exp(struct parser* parser, struct exp_node* parent, struct exp_node* lhs);
bool is_unary_op(struct prototype_node* pnode);
bool is_binary_op(struct prototype_node* pnode);
char get_op_name(struct prototype_node* pnode);
void queue_token(struct parser* parser, struct token tkn);
void queue_tokens(struct parser* psr, struct array* tokens);
struct block_node* parse_file(struct parser* parser, const char* file_name);
struct block_node* parse_file_object(struct parser* parser, const char* mod_name, FILE* file);
struct block_node* parse_repl(struct parser* parser, void (*fun)(void*, struct exp_node*), void* jit);
struct block_node* parse_string(struct parser* parser, const char* mod_name, const char* code);

#ifdef __cplusplus
}
#endif

#endif
