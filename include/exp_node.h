 #pragma once
#include <vector>
#include <string>
using namespace std;
enum NodeType{
    NUMBER_NODE = 1,
    IDENT_NODE,
    VAR_NODE,     //3
    UNARY_NODE,
    BINARY_NODE,  //5
    CONDITION_NODE,
    FOR_NODE,     //7
    CALL_NODE,
    PROTOTYPE_NODE,
    FUNCTION_NODE, //10
    BLOCK_NODE,
};

typedef struct exp_node {
    NodeType type;
    int indent;
    exp_node* parent;
} exp_node;

typedef struct block_node {
    exp_node base;
    vector<exp_node*> nodes;
} block_node;

typedef struct module{
    std::string name;
    block_node* block;
}module;

typedef struct ast{
    std::vector<exp_node*> builtins;
    std::vector<module*> modules;
    module* entry_module;
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
}prototype_node;

typedef struct function_node{
    exp_node base;
    prototype_node* prototype;
    block_node* body;
}function_node;

prototype_node* create_prototype_node(const std::string &name, std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0);