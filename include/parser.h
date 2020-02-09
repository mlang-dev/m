#include "lexer.h"
#include <map>
#include <cassert>
#include "exp_node.h"

typedef struct parser{
    std::map<char, int>* op_precedences;
    token curr_token;
    int curr_token_num;
    ast* ast;
    FILE* file;
}parser;

parser* create_parser(bool create_entry, FILE* file);
void create_builtins(parser* parser, void* context);
void destroy_parser(parser* parser);
int parse_next_token(parser* parser);
exp_node* parse_function(parser* parser, bool has_fun_def_keyword=false);
exp_node* parse_exp_to_function(parser* parser, exp_node* exp=0, const char * fn=0);
exp_node* parse_import(parser* parser);
exp_node* parse_node_with_ident(parser* parser);
exp_node* parse_exp_or_def(parser* parser);

