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
    struct hashtable op_precs;
    struct token curr_token;
    struct ast* ast;
    bool allow_id_as_a_func;
    bool is_repl;
    struct module* current_module;
    struct queue queued_tokens; //queue of token
};

typedef void (*exp_executor)(void*, struct exp_node*);

struct parser* parser_new(const char* file_name, bool is_repl, FILE* file);
void parser_free(struct parser* parser);
void parse_next_token(struct parser* parser);
struct exp_node* parse_exp_to_function(struct parser* parser, struct exp_node* exp, const char* fn);
struct exp_node* parse_import(struct parser* parser, struct exp_node* parent);
struct exp_node* parse_statement(struct parser* parser, struct exp_node* parent);
struct block_node* parse_block(struct parser* parser, struct exp_node* parent, exp_executor exp_exe, void* jit);
struct exp_node* parse_exp(struct parser* parser, struct exp_node* parent, struct exp_node* lhs);
bool is_unary_op(struct prototype_node* pnode);
bool is_binary_op(struct prototype_node* pnode);
char get_op_name(struct prototype_node* pnode);
void queue_token(struct parser* parser, struct token tkn);
void queue_tokens(struct parser* psr, struct array* tokens); //struct array of token

#ifdef __cplusplus
}
#endif

#endif
