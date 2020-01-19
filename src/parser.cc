#include <map>
#include <memory>
#include "parser.h"


int _GetOpPrecedence(Parser* parser);
ExpNode* _ParseNumber(Parser* parser);
ExpNode* _ParseParenExp(Parser* parser);
ExpNode* _ParseIdentExp(Parser* parser);
ExpNode* _ParseNode(Parser* parser);
ExpNode* _ParseBinaryExp(Parser* parser, int exp_prec, ExpNode* lhs);
ExpNode* _ParseExp(Parser* parser);
ExpNode* _ParseCondition(Parser* parser);
ExpNode* _ParseFor(Parser* parser);
ExpNode* _ParseIf(Parser* parser);
ExpNode* _ParseUnary(Parser* parser);
ExpNode* _ParseVar(Parser* parser);
PrototypeNode* _ParsePrototype(Parser* parser);


FunctionNode* _createFunctionNode(PrototypeNode* prototype, ExpNode* body){
    auto node = (FunctionNode*)malloc(sizeof(FunctionNode));
    node->base.type = FunctionType;
    node->_prototype = prototype;
    node->_body = body;
    return node;
}

IdentNode* _createIdentNode(std::string& name){
    auto node = (IdentNode*)malloc(sizeof(IdentNode));
    node->base.type = IdentType;
    node->_name = name;
    return node;
}

NumNode* _createNumNode(double val){
    auto node = (NumNode*)malloc(sizeof(NumNode));
    node->base.type = NumberType;
    node->_val = val;
    return node;
}

VarNode* _createVarNode(const std::vector<std::pair<std::string, ExpNode*>> &var_names,
               ExpNode* body){
    auto node = (VarNode*)malloc(sizeof(VarNode));
    node->base.type = VarType;
    node->_body = body;
    node->_var_names = var_names;
    return node;
}


CallExpNode* _createCallExpNode(const std::string &callee,
                std::vector<ExpNode*> &args){
    auto node = (CallExpNode*)malloc(sizeof(CallExpNode));
    node->base.type = CallType;
    node->_callee = callee;
    node->_args = args;
    return node;
}

PrototypeNode* _createPrototypeNode(const std::string &name, std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0){
    auto node = (PrototypeNode*)malloc(sizeof(PrototypeNode));
    node->base.type = PrototypeType;
    node->_name = name;
    node->_args = args;
    node->_is_operator = is_operator;
    node->_precedence = precedence;
    return node;
}

ConditionNode* _createConditionNode(ExpNode* condition, ExpNode* then, ExpNode* _else){
    auto node = (ConditionNode*)malloc(sizeof(ConditionNode));
    node->base.type = ConditionType;
    node->_condition = condition;
    node->_then = then;
    node->_else = _else;
    return node;
}

UnaryNode* _createUnaryNode(char op, ExpNode* operand){
    auto node = (UnaryNode*)malloc(sizeof(UnaryNode));
    node->base.type = UnaryType;
    node->_op = op;
    node->_operand = operand;
    return node;
}

BinaryNode* _createBinaryNode(char op, ExpNode* lhs, ExpNode* rhs){
    auto node = (BinaryNode*)malloc(sizeof(BinaryNode));
    node->base.type = BinaryType;
    node->_op = op;
    node->_lhs = lhs;
    node->_rhs = rhs;
    return node;
}

ForNode* _createForNode(const std::string &var_name, ExpNode* start, ExpNode* end, ExpNode* step, ExpNode* body){
    auto node = (ForNode*)malloc(sizeof(ForNode));
    node->base.type = ForType;
    node->_var_name = var_name;
    node->_start = start;
    node->_end = end;
    node->_step = step;
    node->_body = body;
    return node;
}

std::map<char, int> OP_PRECEDENCES = {
    {'<', 10},
    {'+', 20},
    {'-', 20},
    {'*', 30},
    {'/', 30}
};

Parser* create_parser(){
    auto parser = (Parser*)malloc(sizeof(Parser));
    parser->op_precedences=&OP_PRECEDENCES;
    return parser;
}

void destroy_parser(Parser* parser){
    free(parser);
}

int advance_to_next_token(Parser* parser){
    //fprintf(stderr, "getting token...\n");
    auto token = get_token();
    parser->_curr_token = token;
    parser->_curr_token_num = token.type == TokenOp?token.op_val : token.type;
    //fprintf(stderr, "got token: %d, %d, %f\n", _curr_token.type, _curr_token.op_val, _curr_token.num_val);
    return parser->_curr_token_num;
}

int _GetOpPrecedence(Parser* parser){
    if(!isascii(parser->_curr_token_num))
        return -1;
    int op_precedence = (*parser->op_precedences)[parser->_curr_token_num];
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

ExpNode* _ParseNumber(Parser* parser){
    auto result = _createNumNode(parser->_curr_token.num_val);
    advance_to_next_token(parser);
    return (ExpNode*)result;
}


ExpNode* _ParseParenExp(Parser* parser){
    advance_to_next_token(parser);
    auto v = _ParseExp(parser);
    if(!v)
        return 0;
    if(parser->_curr_token.op_val!=')')
        return Error("expected ')'");
    advance_to_next_token(parser);
    return v;
}

ExpNode* _ParseIdentExp(Parser* parser){
    std::string id_name = *parser->_curr_token.ident_str;
    advance_to_next_token(parser); //take identifier
    if (parser->_curr_token_num!='(') {//pure variable
        //fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        return (ExpNode*)_createIdentNode(id_name);
    }
    advance_to_next_token(parser);//take next
    std::vector<ExpNode*> args;
    if (parser->_curr_token_num != ')'){
        while(1){
            if(auto arg = _ParseExp(parser))
                args.push_back(arg);
            else
                return 0;
            if (parser->_curr_token_num==')')
                break;
            
            if (parser->_curr_token_num != ',')
                return Error("Expected ')' or ',' in argument list\n");
            advance_to_next_token(parser);
        }
    }
    advance_to_next_token(parser);
    return (ExpNode*)_createCallExpNode(id_name, args);
}


/// varexpr ::= 'var' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
ExpNode* _ParseVar(Parser* parser) {
    advance_to_next_token(parser); // eat the var.
    
    std::vector<std::pair<std::string, ExpNode *> > var_names;
    
    // At least one variable name is required.
    if (parser->_curr_token.type != TokenIdent)
        return Error("expected identifier after var");
    
    while (1) {
        std::string name = *parser->_curr_token.ident_str;
        advance_to_next_token(parser); // eat identifier.
        
        // Read the optional initializer.
        ExpNode *init = 0;
        if (parser->_curr_token_num == '=') {
            advance_to_next_token(parser); // eat the '='.
            
            init = _ParseExp(parser);
            if (init == 0)
                return 0;
        }
        
        var_names.push_back(std::make_pair(name, init));
        
        // End of var list, exit loop.
        if (parser->_curr_token_num != ',')
            break;
        advance_to_next_token(parser); // eat the ','.
        
        if (parser->_curr_token.type != TokenIdent)
            return Error("expected identifier list after var");
    }
    
    // At this point, we have to have 'in'.
    if (parser->_curr_token.type != TokenIn)
        return Error("expected 'in' keyword after 'var'");
    advance_to_next_token(parser); // eat 'in'.
    
    ExpNode *body = _ParseExp(parser);
    if (body == 0)
        return 0;
    
    return (ExpNode*)_createVarNode(var_names, body);
}

ExpNode* _ParseNode(Parser* parser){
    if(parser->_curr_token.type == TokenIdent)
        return _ParseIdentExp(parser);
    else if(parser->_curr_token.type == TokenNum)
        return _ParseNumber(parser);
    else if(parser->_curr_token.type == TokenIf)
        return _ParseIf(parser);
    else if(parser->_curr_token.type == TokenFor)
        return _ParseFor(parser);
    else if(parser->_curr_token.type == TokenVar)
        return _ParseVar(parser);
    else if(parser->_curr_token_num == '(')
        return _ParseParenExp(parser);
    else{
        std::string error = "unknown token: " + std::to_string(parser->_curr_token.type);
        if (parser->_curr_token.type == TokenOp)
            error += " op: " + std::to_string(parser->_curr_token.op_val);
        return Error(error.c_str());
    }
 }

ExpNode* _ParseBinaryExp(Parser* parser, int exp_prec, ExpNode* lhs){
    while(1){
        int tok_prec = _GetOpPrecedence(parser);
        if (tok_prec < exp_prec)
            return lhs;
        
        int binary_op = parser->_curr_token_num;
        advance_to_next_token(parser);
        auto rhs = _ParseUnary(parser); //_ParseNode
        if (!rhs)
            return 0;
        
        auto next_prec = _GetOpPrecedence(parser);
        if (tok_prec < next_prec){
            rhs = _ParseBinaryExp(parser, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        return (ExpNode*)_createBinaryNode(binary_op, lhs, rhs);
    }
}

ExpNode* _ParseExp(Parser* parser){
    auto lhs = _ParseUnary(parser); //_ParseNode
    if (!lhs)
        return 0;
    return _ParseBinaryExp(parser, 0, lhs);
}


/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
PrototypeNode* _ParsePrototype(Parser* parser) {
    std::string fun_name;
    
    //SourceLocation FnLoc = CurLoc;
    
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    int token = 0;
    switch (parser->_curr_token.type) {
        default:
            return ErrorPrototype("Expected function name in prototype");
        case TokenIdent:
            fun_name = *parser->_curr_token.ident_str;
            proto_type = 0;
            advance_to_next_token(parser);
            break;
        case TokenUnary:
            token = advance_to_next_token(parser);
            if (!isascii(token))
                return ErrorPrototype("Expected unary operator");
            fun_name = "unary";
            fun_name += (char)token;
            proto_type = 1;
            advance_to_next_token(parser);
            break;
        case TokenBinary:
            token = advance_to_next_token(parser);
            if (!isascii(token))
                return ErrorPrototype("Expected binary operator");
            fun_name = "binary";
            fun_name += (char)token;
            proto_type = 2;
            advance_to_next_token(parser);
            
            // Read the precedence if present.
            if (parser->_curr_token.type == TokenNum) {
                if (parser->_curr_token.num_val < 1 || parser->_curr_token.num_val > 100)
                    return ErrorPrototype("Invalid precedecnce: must be 1..100");
                bin_prec = (unsigned)parser->_curr_token.num_val;
                advance_to_next_token(parser);
            }
            break;
    }
    /*
    if (_curr_token_num != '(')
        return ErrorPrototype("Expected '(' in prototype");
    */
    std::vector<std::string> arg_names;
    while (parser->_curr_token_num == TokenIdent){
        fprintf(stderr, "arg names: %s", (*parser->_curr_token.ident_str).c_str());
        arg_names.push_back(*parser->_curr_token.ident_str);
        advance_to_next_token(parser);
    }
    fprintf(stderr, "arg names: %d", parser->_curr_token_num);
    /*
    if (_curr_token_num != ')')
        return ErrorPrototype("Expected ')' in prototype");
    */
    // success.
    //advance_to_next_token(); // eat ')'.
    
    // Verify right number of names for operator.
    if (proto_type && arg_names.size() != proto_type)
        return ErrorPrototype("Invalid number of operands for operator");
    return _createPrototypeNode(fun_name, arg_names, proto_type != 0, bin_prec);
}


FunctionNode* parse_function(Parser* parser){
    advance_to_next_token(parser);
    auto prototype = _ParsePrototype(parser);
    if (!prototype)
        return 0;
    if (auto e = _ParseExp(parser))
        return _createFunctionNode(prototype, e);
    return 0;
}

FunctionNode* parse_exp_to_function(Parser* parser){
    if (auto e = _ParseExp(parser)){
        auto args = std::vector<std::string>();
        auto prototype = _createPrototypeNode("main", args);
        return _createFunctionNode(prototype, e);
    }
    return 0;
}

PrototypeNode* parse_import(Parser* parser){
    advance_to_next_token(parser);
    return _ParsePrototype(parser);
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
ExpNode* _ParseCondition(Parser* parser) {
    advance_to_next_token(parser); // eat the if.
    
    // condition.
    ExpNode *cond = _ParseExp(parser);
    if (!cond)
        return 0;
    
    if (parser->_curr_token.type != TokenThen)
        return Error("expected then");
    advance_to_next_token(parser); // eat the then
    
    ExpNode *then = _ParseExp(parser);
    if (!then)
        return 0;
    
    if (parser->_curr_token.type != TokenElse)
        return Error("expected else");
    
    advance_to_next_token(parser);
    
    ExpNode *else_exp = _ParseExp(parser);
    if (!else_exp)
        return 0;
    
    return (ExpNode*)_createConditionNode(cond, then, else_exp);
}

/// unary
///   ::= primary
///   ::= '!' unary
ExpNode *_ParseUnary(Parser* parser) {
    // If the current token is not an operator, it must be a primary expr.
    if (!isascii(parser->_curr_token_num) || parser->_curr_token_num == '(' || parser->_curr_token_num == ','){
        //fprintf(stderr, "parse unary:%d\n", _curr_token_num);
        return _ParseNode(parser);
    }
    // If this is a unary operator, read it.
    int opc = parser->_curr_token.op_val;
    advance_to_next_token(parser);
    if (ExpNode *operand = _ParseUnary(parser))
        return (ExpNode*)_createUnaryNode(opc, operand);
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
ExpNode* _ParseFor(Parser* parser) {
    advance_to_next_token(parser); // eat the for.
    
    if (parser->_curr_token.type != TokenIdent)
        return Error("expected identifier after for");
    
    std::string id_name = *parser->_curr_token.ident_str;
    advance_to_next_token(parser); // eat identifier.
    
    if (parser->_curr_token.op_val != '=')
        return Error("expected '=' after for");
    advance_to_next_token(parser); // eat '='.
    
    ExpNode *start = _ParseExp(parser);
    if (start == 0)
        return 0;
    if (parser->_curr_token.op_val != ',')
        return Error("expected ',' after for start value");
    advance_to_next_token(parser);
    
    ExpNode *end = _ParseExp(parser);
    if (end == 0)
        return 0;
    
    // The step value is optional.
    ExpNode *step = 0;
    if (parser->_curr_token.op_val == ',') {
        advance_to_next_token(parser);
        step = _ParseExp(parser);
        if (step == 0)
            return 0;
    }
    
    if (parser->_curr_token.type != TokenIn)
        return Error("expected 'in' after for");
    advance_to_next_token(parser); // eat 'in'.
    
    ExpNode* body = _ParseExp(parser);
    if (body == 0)
        return 0;
    
    return (ExpNode*)_createForNode(id_name, start, end, step, body);
}

ExpNode* _ParseIf(Parser* parser) {
    advance_to_next_token(parser); // eat the if.
    
    // condition.
    ExpNode *cond = _ParseExp(parser);
    if (!cond)
        return 0;
    
    if (parser->_curr_token.type != TokenThen)
        return Error("expected then");
    advance_to_next_token(parser); // eat the then
    
    ExpNode *then = _ParseExp(parser);
    if (then == 0)
        return 0;
    
    if (parser->_curr_token.type != TokenElse)
        return Error("expected else");
    
    advance_to_next_token(parser);
    
    ExpNode * else_exp = _ParseExp(parser);
    if (!else_exp)
        return 0;
    
    return (ExpNode*)_createConditionNode(cond, then, else_exp);
}
