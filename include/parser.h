#pragma once
#include "lexer.h"
#include <map>
#include <vector>
#include <cassert>
#include "ast.h"

using namespace std;
struct parser{
    map<string, int>* op_precedences;
    token curr_token;
    ast* ast;
    bool allow_id_as_a_func;
    bool is_repl;
    module* current_module;
    queue<token> queued_tokens;
};

parser* create_parser(const char* file_name, bool is_repl, FILE* (*open_file)(const char* file_name)=nullptr);
void create_builtins(parser* parser, void* context);
void destroy_parser(parser* parser);
void parse_next_token(parser* parser);
exp_node* parse_exp_to_function(parser* parser, exp_node* exp=0, const char * fn=0);
exp_node* parse_import(parser* parser, exp_node* parent);
exp_node* parse_statement(parser* parser, exp_node* parent);
block_node *parse_block(parser *parser, exp_node *parent, void (*fun)(void*, exp_node*) = nullptr, void*jit=nullptr);
exp_node *parse_exp(parser *parser, exp_node* parent, exp_node *lhs = 0);
bool is_unary_op(prototype_node* pnode);
bool is_binary_op(prototype_node* pnode);
char get_op_name(prototype_node* pnode);
void queue_token(parser* parser, token tkn);
void queue_tokens(parser* psr, vector<token> tokens);
