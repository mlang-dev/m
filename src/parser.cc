#include <map>
#include <memory>
#include "parser.h"


Parser::Parser(){
    _op_precedence['<'] = 10;
    _op_precedence['+'] = 20;
    _op_precedence['-'] = 20;
    _op_precedence['*'] = 40;
    _op_precedence['/'] = 40;

}

int Parser::AdvanceToNextToken(){
    //fprintf(stderr, "getting token...\n");
    _curr_token = GetToken();
    _curr_token_num = _curr_token.type == TokenOp?_curr_token.op_val : _curr_token.type;
    return _curr_token_num;
    //fprintf(stderr, "got token: %d, %d, %f\n", curr_token.type, curr_token.op_val, curr_token.num_val);
}

int Parser::_GetOpPrecedence(){
    if(!isascii(_curr_token_num))
        return -1;
    int op_precedence = _op_precedence[_curr_token_num];
    //fprintf(stderr, "op %d: pre: %d\n", op, op_precedence);
    if (op_precedence <= 0)
        return -1;
    return op_precedence;
}

ExpNode* Error(const char* str){
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

PrototypeNode* ErrorPrototype(const char * str) {
    Error(str);
    return 0;
}

ExpNode* Parser::_ParseNumber(){
    auto result = new NumNode(_curr_token.num_val);
    AdvanceToNextToken();
    return result;
}


ExpNode* Parser::_ParseParenExp(){
    AdvanceToNextToken();
    auto v = _ParseExp();
    if(!v)
        return 0;
    if(_curr_token.op_val!=')')
        return Error("expected ')'");
    AdvanceToNextToken();
    return v;
}

ExpNode* Parser::_ParseIdentExp(){
    std::string id_name = *_curr_token.ident_str;
    AdvanceToNextToken(); //take identifier
    if (_curr_token_num!='(') {//pure variable
        //fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        return new IdentNode(id_name);
    }
    AdvanceToNextToken();//take next
    std::vector<ExpNode*> args;
    if (_curr_token_num != ')'){
        while(1){
            if(auto arg = _ParseExp())
                args.push_back(arg);
            else
                return 0;
            if (_curr_token_num==')')
                break;
            
            if (_curr_token_num != ',')
                return Error("Expected ')' or ',' in argument list\n");
            AdvanceToNextToken();
        }
    }
    AdvanceToNextToken();
    return new CallExpNode(id_name, args);
}


/// varexpr ::= 'var' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
ExpNode* Parser::_ParseVar() {
    AdvanceToNextToken(); // eat the var.
    
    std::vector<std::pair<std::string, ExpNode *> > var_names;
    
    // At least one variable name is required.
    if (_curr_token.type != TokenIdent)
        return Error("expected identifier after var");
    
    while (1) {
        std::string name = *_curr_token.ident_str;
        AdvanceToNextToken(); // eat identifier.
        
        // Read the optional initializer.
        ExpNode *init = 0;
        if (_curr_token_num == '=') {
            AdvanceToNextToken(); // eat the '='.
            
            init = _ParseExp();
            if (init == 0)
                return 0;
        }
        
        var_names.push_back(std::make_pair(name, init));
        
        // End of var list, exit loop.
        if (_curr_token_num != ',')
            break;
        AdvanceToNextToken(); // eat the ','.
        
        if (_curr_token.type != TokenIdent)
            return Error("expected identifier list after var");
    }
    
    // At this point, we have to have 'in'.
    if (_curr_token.type != TokenIn)
        return Error("expected 'in' keyword after 'var'");
    AdvanceToNextToken(); // eat 'in'.
    
    ExpNode *body = _ParseExp();
    if (body == 0)
        return 0;
    
    return new VarNode(var_names, body);
}

ExpNode* Parser::_ParseNode(){
    if(_curr_token.type == TokenIdent)
        return _ParseIdentExp();
    else if(_curr_token.type == TokenNum)
        return _ParseNumber();
    else if(_curr_token.type == TokenIf)
        return _ParseIf();
    else if(_curr_token.type == TokenFor)
        return _ParseFor();
    else if(_curr_token.type == TokenVar)
        return _ParseVar();
    else if(_curr_token_num == '(')
        return _ParseParenExp();
    else{
        std::string error = "unknown token: " + std::to_string(_curr_token.type);
        if (_curr_token.type == TokenOp)
            error += " op: " + std::to_string(_curr_token.op_val);
        return Error(error.c_str());
    }
 }

ExpNode* Parser::_ParseBinaryExp(int exp_prec, ExpNode* lhs){
    while(1){
        int tok_prec = _GetOpPrecedence();
        if (tok_prec < exp_prec)
            return lhs;
        
        int binary_op = _curr_token_num;
        AdvanceToNextToken();
        auto rhs = _ParseUnary(); //_ParseNode
        if (!rhs)
            return 0;
        
        auto next_prec = _GetOpPrecedence();
        if (tok_prec < next_prec){
            rhs = _ParseBinaryExp(tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        
        lhs = new BinaryNode(binary_op, lhs, rhs);
    }
}

ExpNode* Parser::_ParseExp(){
    auto lhs = _ParseUnary(); //_ParseNode
    if (!lhs)
        return 0;
    return _ParseBinaryExp(0, lhs);
}


/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
PrototypeNode* Parser::_ParsePrototype() {
    std::string fun_name;
    
    //SourceLocation FnLoc = CurLoc;
    
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    int token = 0;
    switch (_curr_token.type) {
        default:
            return ErrorPrototype("Expected function name in prototype");
        case TokenIdent:
            fun_name = *_curr_token.ident_str;
            proto_type = 0;
            AdvanceToNextToken();
            break;
        case TokenUnary:
            token = AdvanceToNextToken();
            if (!isascii(token))
                return ErrorPrototype("Expected unary operator");
            fun_name = "unary";
            fun_name += (char)token;
            proto_type = 1;
            AdvanceToNextToken();
            break;
        case TokenBinary:
            token = AdvanceToNextToken();
            if (!isascii(token))
                return ErrorPrototype("Expected binary operator");
            fun_name = "binary";
            fun_name += (char)token;
            proto_type = 2;
            AdvanceToNextToken();
            
            // Read the precedence if present.
            if (_curr_token.type == TokenNum) {
                if (_curr_token.num_val < 1 || _curr_token.num_val > 100)
                    return ErrorPrototype("Invalid precedecnce: must be 1..100");
                bin_prec = (unsigned)_curr_token.num_val;
                AdvanceToNextToken();
            }
            break;
    }
    /*
    if (_curr_token_num != '(')
        return ErrorPrototype("Expected '(' in prototype");
    */
    std::vector<std::string> arg_names;
    while (_curr_token_num == TokenIdent){
        fprintf(stderr, "arg names: %s", (*_curr_token.ident_str).c_str());
        arg_names.push_back(*_curr_token.ident_str);
        AdvanceToNextToken();
    }
    fprintf(stderr, "arg names: %d", _curr_token_num);
    /*
    if (_curr_token_num != ')')
        return ErrorPrototype("Expected ')' in prototype");
    */
    // success.
    //AdvanceToNextToken(); // eat ')'.
    
    // Verify right number of names for operator.
    if (proto_type && arg_names.size() != proto_type)
        return ErrorPrototype("Invalid number of operands for operator");
    
    return new PrototypeNode(fun_name, arg_names, proto_type != 0, bin_prec);
}

FunctionNode* Parser::ParseFunction(){
    AdvanceToNextToken();
    auto prototype = _ParsePrototype();
    if (!prototype)
        return 0;
    if (auto e = _ParseExp())
        return new FunctionNode(prototype, e);
    return 0;
}

FunctionNode* Parser::ParseExpToFunction(){
    if (auto e = _ParseExp()){
        auto args = std::vector<std::string>();
        auto prototype = new PrototypeNode("main", args);
        return new FunctionNode(prototype, e);
    }
    return 0;
}

PrototypeNode* Parser::ParseImport(){
    AdvanceToNextToken();
    return _ParsePrototype();
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
ExpNode* Parser::_ParseCondition() {
    AdvanceToNextToken(); // eat the if.
    
    // condition.
    ExpNode *cond = _ParseExp();
    if (!cond)
        return 0;
    
    if (_curr_token.type != TokenThen)
        return Error("expected then");
    AdvanceToNextToken(); // eat the then
    
    ExpNode *then = _ParseExp();
    if (!then)
        return 0;
    
    if (_curr_token.type != TokenElse)
        return Error("expected else");
    
    AdvanceToNextToken();
    
    ExpNode *else_exp = _ParseExp();
    if (!else_exp)
        return 0;
    
    return new ConditionNode(cond, then, else_exp);
}

/// unary
///   ::= primary
///   ::= '!' unary
ExpNode *Parser::_ParseUnary() {
    // If the current token is not an operator, it must be a primary expr.
    if (!isascii(_curr_token_num) || _curr_token_num == '(' || _curr_token_num == ','){
        //fprintf(stderr, "parse unary:%d\n", _curr_token_num);
        return _ParseNode();
    }
    // If this is a unary operator, read it.
    int opc = _curr_token.op_val;
    AdvanceToNextToken();
    if (ExpNode *operand = _ParseUnary())
        return new UnaryNode(opc, operand);
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
ExpNode* Parser::_ParseFor() {
    AdvanceToNextToken(); // eat the for.
    
    if (_curr_token.type != TokenIdent)
        return Error("expected identifier after for");
    
    std::string id_name = *_curr_token.ident_str;
    AdvanceToNextToken(); // eat identifier.
    
    if (_curr_token.op_val != '=')
        return Error("expected '=' after for");
    AdvanceToNextToken(); // eat '='.
    
    ExpNode *start = _ParseExp();
    if (start == 0)
        return 0;
    if (_curr_token.op_val != ',')
        return Error("expected ',' after for start value");
    AdvanceToNextToken();
    
    ExpNode *end = _ParseExp();
    if (end == 0)
        return 0;
    
    // The step value is optional.
    ExpNode *step = 0;
    if (_curr_token.op_val == ',') {
        AdvanceToNextToken();
        step = _ParseExp();
        if (step == 0)
            return 0;
    }
    
    if (_curr_token.type != TokenIn)
        return Error("expected 'in' after for");
    AdvanceToNextToken(); // eat 'in'.
    
    ExpNode* body = _ParseExp();
    if (body == 0)
        return 0;
    
    return new ForNode(id_name, start, end, step, body);
}

ExpNode* Parser::_ParseIf() {
    AdvanceToNextToken(); // eat the if.
    
    // condition.
    ExpNode *cond = _ParseExp();
    if (!cond)
        return 0;
    
    if (_curr_token.type != TokenThen)
        return Error("expected then");
    AdvanceToNextToken(); // eat the then
    
    ExpNode *then = _ParseExp();
    if (then == 0)
        return 0;
    
    if (_curr_token.type != TokenElse)
        return Error("expected else");
    
    AdvanceToNextToken();
    
    ExpNode * else_exp = _ParseExp();
    if (!else_exp)
        return 0;
    
    return new ConditionNode(cond, then, else_exp);
}
