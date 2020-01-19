#include "lexer.h"
#include <vector>
#include <map>
#include <cassert>

enum NodeType{
    NumberType = 1,
    IdentType,
    VarType,
    UnaryType,
    BinaryType,
    ConditionType,
    ForType,
    CallType,
    PrototypeType,
    FunctionType
};

typedef struct ExpNode {
    NodeType type;
} ExpNode;

typedef struct NumNode{
    ExpNode base;
    double _val;
} NumNode;

typedef struct IdentNode {
    ExpNode base;
    std::string _name;
} IdentNode;

typedef struct VarNode {
    ExpNode base;
    std::vector<std::pair<std::string, ExpNode *> > _var_names;
    ExpNode *_body;
}VarNode;


typedef struct UnaryNode{
    ExpNode base;
    char _op;
    ExpNode* _operand;
}UnaryNode;

typedef struct BinaryNode{
    ExpNode base;
    char _op;
    ExpNode *_lhs, *_rhs;
}BinaryNode;

typedef struct  ConditionNode{
    ExpNode base;
    ExpNode *_condition, *_then, *_else;
}ConditionNode;

typedef struct ForNode{
    ExpNode base;
    std::string _var_name;
    ExpNode *_start, *_end, *_step, *_body;
}ForNode;

typedef struct CallExpNode{
    ExpNode base;
    std::string _callee;
    std::vector<ExpNode*> _args;
}CallExpNode;


typedef struct PrototypeNode{
    ExpNode base;
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
}PrototypeNode;


typedef struct FunctionNode{
    ExpNode base;
    PrototypeNode* _prototype;
    ExpNode* _body;
}FunctionNode;

typedef struct Parser{
    std::map<char, int>* op_precedences;
    Token _curr_token;
    int _curr_token_num;
}Parser;

Parser* create_parser();
void destroy_parser(Parser* parser);

int advance_to_next_token(Parser* parser);
FunctionNode* parse_function(Parser* parser);
FunctionNode* parse_exp_to_function(Parser* parser);
PrototypeNode* parse_import(Parser* parser);

