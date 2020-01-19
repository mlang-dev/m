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

typedef struct num_node{
    exp_node base;
    double _val;
} num_node;

typedef struct ident_node {
    exp_node base;
    std::string _name;
} ident_node;

typedef struct var_node {
    exp_node base;
    std::vector<std::pair<std::string, exp_node *> > _var_names;
    exp_node *_body;
}var_node;


typedef struct unary_node{
    exp_node base;
    char _op;
    exp_node* _operand;
}unary_node;

typedef struct binary_node{
    exp_node base;
    char _op;
    exp_node *_lhs, *_rhs;
}binary_node;

typedef struct  condition_node{
    exp_node base;
    exp_node *_condition, *_then, *_else;
}condition_node;

typedef struct for_node{
    exp_node base;
    std::string _var_name;
    exp_node *_start, *_end, *_step, *_body;
}for_node;

typedef struct call_node{
    exp_node base;
    std::string _callee;
    std::vector<exp_node*> _args;
}call_node;


typedef struct prototype_node{
    exp_node base;
    std::string _name;
    std::vector<std::string> _args;
    bool _is_operator;
    unsigned _precedence;
    
    bool IsUnaryOp() const { return _is_operator && _args.size() == 1;}
    bool isBinaryOp() const { return _is_operator && _args.size() == 2;}
    
    char GetOpName() const {
        assert(IsUnaryOp() || isBinaryOp());
        return _name[_name.size()-1];
    }
}prototype_node;


typedef struct function_node{
    exp_node base;
    prototype_node* _prototype;
    exp_node* _body;
}function_node;

typedef struct parser{
    std::map<char, int>* op_precedences;
    token _curr_token;
    int _curr_token_num;
}parser;

parser* create_parser();
void destroy_parser(parser* parser);

int advance_to_next_token(parser* parser);
function_node* parse_function(parser* parser);
function_node* parse_exp_to_function(parser* parser);
prototype_node* parse_import(parser* parser);

