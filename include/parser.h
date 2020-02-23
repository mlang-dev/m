#include "lexer.h"
#include <map>
#include <cassert>
#include "exp_node.h"

typedef struct parser{
    std::map<char, int>* op_precedences;
    token curr_token;
    ast* ast;
    FILE* file;
    bool allow_id_as_a_func;
    bool is_repl;
}parser;

parser* create_parser(bool create_entry, FILE* file, bool is_repl);
void create_builtins(parser* parser, void* context);
void destroy_parser(parser* parser);
void parse_next_token(parser* parser);
exp_node* parse_exp_to_function(parser* parser, exp_node* exp=0, const char * fn=0);
exp_node* parse_import(parser* parser, exp_node* parent);
exp_node* parse_statement(parser* parser, exp_node* parent);
block_node *parse_block(parser *parser, exp_node *parent, void (*fun)(void*, exp_node*) = nullptr, void*jit=nullptr);

