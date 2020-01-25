#include "lexer.h"
#include <map>
#include <cassert>
#include "exp_node.h"

typedef struct parser{
    std::map<char, int>* op_precedences;
    token curr_token;
    int curr_token_num;
    ast* ast;
}parser;

parser* create_parser(bool create_entry);
void create_builtins(parser* parser, void* context);
void destroy_parser(parser* parser);
int parse_next_token(parser* parser);
exp_node* parse_function(parser* parser);
exp_node* parse_exp_to_function(parser* parser);
exp_node* parse_import(parser* parser);
