#include "lexer.h"
#include <vector>
#include <map>
#include <cassert>

enum NodeType{
    NUMBER_NODE = 1,
    IDENT_NODE,
    VAR_NODE,
    UNARY_NODE,
    BINARY_NODE,
    CONDITION_NODE,
    FOR_NODE,
    CALL_NODE,
    PROTOTYPE_NODE,
    FUNCTION_NODE
};

typedef struct exp_node {
    NodeType type;
} exp_node;

typedef struct module{
    std::vector<exp_node*> nodes;
}module;

typedef struct ast{
    std::vector<exp_node*> builtins;
    std::vector<module*> modules;
    exp_node* entry;
}ast;

typedef struct num_node{
    exp_node base;
    double num_val;
}num_node;

typedef struct ident_node {
    exp_node base;
    std::string name;
}ident_node;

typedef struct var_node {
    exp_node base;
    std::vector<std::pair<std::string, exp_node *> > var_names;
    exp_node *body;
}var_node;


typedef struct unary_node{
    exp_node base;
    char op;
    exp_node* operand;
}unary_node;

typedef struct binary_node{
    exp_node base;
    char op;
    exp_node *lhs, *rhs;
}binary_node;

typedef struct  condition_node{
    exp_node base;
    exp_node *condition_node, *then_node, *else_node;
}condition_node;

typedef struct for_node{
    exp_node base;
    std::string var_name;
    exp_node *start, *end, *step, *body;
}for_node;

typedef struct call_node{
    exp_node base;
    std::string callee;
    std::vector<exp_node*> args;
}call_node;

typedef struct prototype_node{
    exp_node base;
    std::string name;
    std::vector<std::string> args;
    bool is_operator;
    unsigned precedence;
    bool is_a_value;
}prototype_node;

typedef struct function_node{
    exp_node base;
    prototype_node* prototype;
    exp_node* body;
}function_node;

typedef struct parser{
    std::map<char, int>* op_precedences;
    token curr_token;
    int curr_token_num;
    ast* ast;
}parser;

parser* create_parser();
void destroy_parser(parser* parser);
int parse_next_token(parser* parser);
exp_node* parse_function(parser* parser);
exp_node* parse_exp_to_function(parser* parser);
exp_node* parse_import(parser* parser);
