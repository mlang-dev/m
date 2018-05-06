#include "lexer.h"
#include <vector>
#include <map>
#include <cassert>

class CodeGenerator
{
public:
    virtual void* generate(class NumNode* node) = 0;
    virtual void* generate(class IdentNode* node) = 0;
    virtual void* generate(class VarNode* node) = 0;
    virtual void* generate(class BinaryNode* node) = 0;
    virtual void* generate(class CallExpNode* node) = 0;
    virtual void* generate(class PrototypeNode* node) = 0;
    virtual void* generate(class FunctionNode* node) = 0;
    virtual void* generate(class ConditionNode* node) = 0;
    virtual void* generate(class ForNode* node) = 0;
    virtual void* generate(class UnaryNode* node) = 0;
};


class ExpNode{
public:
    virtual ~ExpNode(){}
    virtual void* codegen(CodeGenerator* generator) = 0;
};

class NumNode : public ExpNode{
public:
    double _val;
public:
    NumNode(double val) : _val(val){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class IdentNode : public ExpNode{
public:
    std::string _name;
public:
    IdentNode(std::string& name): _name(name){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class VarNode : public ExpNode {
public:
    std::vector<std::pair<std::string, ExpNode *> > _var_names;
    ExpNode *_body;
    
public:
    VarNode(const std::vector<std::pair<std::string, ExpNode *> > &var_names,
               ExpNode *body)
    : _var_names(var_names), _body(body) {}
    
    
    void *codegen(CodeGenerator* generator) override { return generator->generate(this);}
};


class UnaryNode : public ExpNode{
public:
    char _op;
    ExpNode* _operand;
public:
    UnaryNode(char op, ExpNode* operand): _op(op), _operand(operand){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class BinaryNode : public ExpNode{
public:
    char _op;
    ExpNode *_lhs, *_rhs;
public:
    BinaryNode(char op, ExpNode* lhs, ExpNode* rhs) :
    _op(op), _lhs(lhs), _rhs(rhs){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class ConditionNode : public ExpNode{
public:
    ExpNode *_condition, *_then, *_else;
public:
    ConditionNode(ExpNode* condition, ExpNode*then, ExpNode*__else):
    _condition(condition), _then(then), _else(__else){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class ForNode : public ExpNode{
public:
    std::string _var_name;
    ExpNode *_start, *_end, *_step, *_body;
public:
    ForNode(const std::string &var_name, ExpNode* start, ExpNode* end, ExpNode* step, ExpNode* body):
    _var_name(var_name), _start(start), _end(end), _step(step), _body(body){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};

class CallExpNode : public ExpNode{
public:
    std::string _callee;
    std::vector<ExpNode*> _args;
public:
    CallExpNode(const std::string &callee,
                std::vector<ExpNode*> &args)
    :_callee(callee), _args(args){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
    
};


class PrototypeNode : public ExpNode{
public:
    std::string _name;
    std::vector<std::string> _args;
    bool _is_operator;
    unsigned _precedence;
public:
    PrototypeNode(const std::string &name, std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0):
    _name(name), _args(args), _is_operator(is_operator), _precedence(precedence){}
    
    bool IsUnaryOp() const { return _is_operator && _args.size() == 1;}
    bool isBinaryOp() const { return _is_operator && _args.size() == 2;}
    
    char GetOpName() const {
        assert(IsUnaryOp() || isBinaryOp());
        return _name[_name.size()-1];
    }
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};


class FunctionNode : public ExpNode{
public:
    PrototypeNode* _prototype;
    ExpNode* _body;
    
public:
    FunctionNode(PrototypeNode* prototype, ExpNode* body):
    _prototype(prototype), _body(body){}
    void* codegen(CodeGenerator* generator) override { return generator->generate(this);}
};


class Parser{
    
    int _GetOpPrecedence();
    ExpNode* _ParseNumber();
    ExpNode* _ParseParenExp();
    ExpNode* _ParseIdentExp();
    ExpNode* _ParseNode();
    ExpNode* _ParseBinaryExp(int exp_prec, ExpNode* lhs);
    ExpNode* _ParseExp();
    ExpNode* _ParseCondition();
    ExpNode* _ParseFor();
    ExpNode* _ParseIf();
    ExpNode* _ParseUnary();
    ExpNode* _ParseVar();
    PrototypeNode* _ParsePrototype();
    
public:
    std::map<char, int> _op_precedence;
    Parser();
    Token _curr_token;
    int _curr_token_num;
    int AdvanceToNextToken();
    FunctionNode* ParseFunction();
    FunctionNode* ParseExpToFunction();
    PrototypeNode* ParseImport();
};

