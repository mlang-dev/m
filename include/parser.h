/*
 * parser.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m parser
 */
#ifndef __MLANG_PARSER_H__
#define __MLANG_PARSER_H__

#include <map>
#include <vector>
#include <queue>

#include "clib/hashtable.h"

#include "ast.h"
#include "lexer.h"

struct parser {
    hashtable op_precs;
    token curr_token;
    ast* ast;
    bool allow_id_as_a_func;
    bool is_repl;
    module* current_module;
    std::queue<token> queued_tokens;
};

parser* parser_new(const char* file_name, bool is_repl, FILE* (*open_file)(const char* file_name) = nullptr);
void create_builtins(parser* parser, void* context);
void parser_free(parser* parser);
void parse_next_token(parser* parser);
exp_node* parse_exp_to_function(parser* parser, exp_node* exp = 0, const char* fn = 0);
exp_node* parse_import(parser* parser, exp_node* parent);
exp_node* parse_statement(parser* parser, exp_node* parent);
block_node* parse_block(parser* parser, exp_node* parent, void (*fun)(void*, exp_node*) = nullptr, void* jit = nullptr);
exp_node* parse_exp(parser* parser, exp_node* parent, exp_node* lhs = 0);
bool is_unary_op(prototype_node* pnode);
bool is_binary_op(prototype_node* pnode);
char get_op_name(prototype_node* pnode);
void queue_token(parser* parser, token tkn);
void queue_tokens(parser* psr, std::vector<token> tokens);

#endif