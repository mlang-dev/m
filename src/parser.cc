#include <map>
#include <memory>
#include "parser.h"


int _GetOpPrecedence(parser* parser);
exp_node* _ParseNumber(parser* parser);
exp_node* _ParseParenExp(parser* parser);
exp_node* _ParseIdentExp(parser* parser);
exp_node* _ParseNode(parser* parser);
exp_node* _ParseBinaryExp(parser* parser, int exp_prec, exp_node* lhs);
exp_node* _ParseExp(parser* parser);
exp_node* _ParseCondition(parser* parser);
exp_node* _ParseFor(parser* parser);
exp_node* _ParseIf(parser* parser);
exp_node* _ParseUnary(parser* parser);
exp_node* _ParseVar(parser* parser);
prototype_node* _ParsePrototype(parser* parser);


function_node* _createFunctionNode(prototype_node* prototype, exp_node* body){
    auto node = (function_node*)malloc(sizeof(function_node));
    node->base.type = FUNCTION_NODE;
    node->_prototype = prototype;
    node->_body = body;
    return node;
}

ident_node* _createIdentNode(std::string& name){
    auto node = (ident_node*)malloc(sizeof(ident_node));
    node->base.type = IDENT_NODE;
    node->_name = name;
    return node;
}

num_node* _createNumNode(double val){
    auto node = (num_node*)malloc(sizeof(num_node));
    node->base.type = NUMBER_NODE;
    node->_val = val;
    return node;
}

var_node* _createVarNode(const std::vector<std::pair<std::string, exp_node*>> &var_names,
               exp_node* body){
    auto node = (var_node*)malloc(sizeof(var_node));
    node->base.type = VAR_NODE;
    node->_body = body;
    node->_var_names = var_names;
    return node;
}


call_node* _createCallExpNode(const std::string &callee,
                std::vector<exp_node*> &args){
    auto node = (call_node*)malloc(sizeof(call_node));
    node->base.type = CALL_NODE;
    node->_callee = callee;
    node->_args = args;
    return node;
}

prototype_node* _createPrototypeNode(const std::string &name, std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0){
    auto node = (prototype_node*)malloc(sizeof(prototype_node));
    node->base.type = PROTOTYPE_NODE;
    node->_name = name;
    node->_args = args;
    node->_is_operator = is_operator;
    node->_precedence = precedence;
    return node;
}

condition_node* _createConditionNode(exp_node* condition, exp_node* then, exp_node* _else){
    auto node = (condition_node*)malloc(sizeof(condition_node));
    node->base.type = CONDITION_NODE;
    node->_condition = condition;
    node->_then = then;
    node->_else = _else;
    return node;
}

unary_node* _createUnaryNode(char op, exp_node* operand){
    auto node = (unary_node*)malloc(sizeof(unary_node));
    node->base.type = UNARY_NODE;
    node->_op = op;
    node->_operand = operand;
    return node;
}

binary_node* _createBinaryNode(char op, exp_node* lhs, exp_node* rhs){
    auto node = (binary_node*)malloc(sizeof(binary_node));
    node->base.type = BINARY_NODE;
    node->_op = op;
    node->_lhs = lhs;
    node->_rhs = rhs;
    return node;
}

for_node* _createForNode(const std::string &var_name, exp_node* start, exp_node* end, exp_node* step, exp_node* body){
    auto node = (for_node*)malloc(sizeof(for_node));
    node->base.type = FOR_NODE;
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

parser* create_parser(){
    auto psr = (parser*)malloc(sizeof(parser));
    psr->op_precedences=&OP_PRECEDENCES;
    return psr;
}

void destroy_parser(parser* parser){
    free(parser);
}

int advance_to_next_token(parser* parser){
    //fprintf(stderr, "getting token...\n");
    auto token = get_token();
    parser->_curr_token = token;
    parser->_curr_token_num = token.type == TOKEN_OP?token.op_val : token.type;
    //fprintf(stderr, "got token: %d, %d, %f\n", _curr_token.type, _curr_token.op_val, _curr_token.num_val);
    return parser->_curr_token_num;
}

int _GetOpPrecedence(parser* parser){
    if(!isascii(parser->_curr_token_num))
        return -1;
    int op_precedence = (*parser->op_precedences)[parser->_curr_token_num];
    //fprintf(stderr, "op %d: pre: %d\n", op, op_precedence);
    if (op_precedence <= 0)
        return -1;
    return op_precedence;
}

exp_node* Error(const char* str){
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}

prototype_node* ErrorPrototype(const char * str) {
    Error(str);
    return 0;
}

exp_node* _ParseNumber(parser* parser){
    auto result = _createNumNode(parser->_curr_token.num_val);
    advance_to_next_token(parser);
    return (exp_node*)result;
}


exp_node* _ParseParenExp(parser* parser){
    advance_to_next_token(parser);
    auto v = _ParseExp(parser);
    if(!v)
        return 0;
    if(parser->_curr_token.op_val!=')')
        return Error("expected ')'");
    advance_to_next_token(parser);
    return v;
}

exp_node* _ParseIdentExp(parser* parser){
    std::string id_name = *parser->_curr_token.ident_str;
    advance_to_next_token(parser); //take identifier
    if (parser->_curr_token_num!='(') {//pure variable
        //fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        return (exp_node*)_createIdentNode(id_name);
    }
    advance_to_next_token(parser);//take next
    std::vector<exp_node*> args;
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
    return (exp_node*)_createCallExpNode(id_name, args);
}


/// varexpr ::= 'var' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
exp_node* _ParseVar(parser* parser) {
    advance_to_next_token(parser); // eat the var.
    
    std::vector<std::pair<std::string, exp_node *> > var_names;
    
    // At least one variable name is required.
    if (parser->_curr_token.type != TOKEN_IDENT)
        return Error("expected identifier after var");
    
    while (1) {
        std::string name = *parser->_curr_token.ident_str;
        advance_to_next_token(parser); // eat identifier.
        
        // Read the optional initializer.
        exp_node *init = 0;
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
        
        if (parser->_curr_token.type != TOKEN_IDENT)
            return Error("expected identifier list after var");
    }
    
    // At this point, we have to have 'in'.
    if (parser->_curr_token.type != TOKEN_IN)
        return Error("expected 'in' keyword after 'var'");
    advance_to_next_token(parser); // eat 'in'.
    
    exp_node *body = _ParseExp(parser);
    if (body == 0)
        return 0;
    
    return (exp_node*)_createVarNode(var_names, body);
}

exp_node* _ParseNode(parser* parser){
    if(parser->_curr_token.type == TOKEN_IDENT)
        return _ParseIdentExp(parser);
    else if(parser->_curr_token.type == TOKEN_NUM)
        return _ParseNumber(parser);
    else if(parser->_curr_token.type == TOKEN_IF)
        return _ParseIf(parser);
    else if(parser->_curr_token.type == TOKEN_FOR)
        return _ParseFor(parser);
    else if(parser->_curr_token.type == TOKEN_VAR)
        return _ParseVar(parser);
    else if(parser->_curr_token_num == '(')
        return _ParseParenExp(parser);
    else{
        std::string error = "unknown token: " + std::to_string(parser->_curr_token.type);
        if (parser->_curr_token.type == TOKEN_OP)
            error += " op: " + std::to_string(parser->_curr_token.op_val);
        return Error(error.c_str());
    }
 }

exp_node* _ParseBinaryExp(parser* parser, int exp_prec, exp_node* lhs){
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
        return (exp_node*)_createBinaryNode(binary_op, lhs, rhs);
    }
}

exp_node* _ParseExp(parser* parser){
    auto lhs = _ParseUnary(parser); //_ParseNode
    if (!lhs)
        return 0;
    return _ParseBinaryExp(parser, 0, lhs);
}


/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
prototype_node* _ParsePrototype(parser* parser) {
    std::string fun_name;
    
    //SourceLocation FnLoc = CurLoc;
    
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    int token = 0;
    switch (parser->_curr_token.type) {
        default:
            return ErrorPrototype("Expected function name in prototype");
        case TOKEN_IDENT:
            fun_name = *parser->_curr_token.ident_str;
            proto_type = 0;
            advance_to_next_token(parser);
            break;
        case TOKEN_UNARY:
            token = advance_to_next_token(parser);
            if (!isascii(token))
                return ErrorPrototype("Expected unary operator");
            fun_name = "unary";
            fun_name += (char)token;
            proto_type = 1;
            advance_to_next_token(parser);
            break;
        case TOKEN_BINARY:
            token = advance_to_next_token(parser);
            if (!isascii(token))
                return ErrorPrototype("Expected binary operator");
            fun_name = "binary";
            fun_name += (char)token;
            proto_type = 2;
            advance_to_next_token(parser);
            
            // Read the precedence if present.
            if (parser->_curr_token.type == TOKEN_NUM) {
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
    while (parser->_curr_token_num == TOKEN_IDENT){
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


function_node* parse_function(parser* parser){
    advance_to_next_token(parser);
    auto prototype = _ParsePrototype(parser);
    if (!prototype)
        return 0;
    if (auto e = _ParseExp(parser))
        return _createFunctionNode(prototype, e);
    return 0;
}

function_node* parse_exp_to_function(parser* parser){
    if (auto e = _ParseExp(parser)){
        auto args = std::vector<std::string>();
        auto prototype = _createPrototypeNode("main", args);
        return _createFunctionNode(prototype, e);
    }
    return 0;
}

prototype_node* parse_import(parser* parser){
    advance_to_next_token(parser);
    return _ParsePrototype(parser);
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
exp_node* _ParseCondition(parser* parser) {
    advance_to_next_token(parser); // eat the if.
    
    // condition.
    exp_node *cond = _ParseExp(parser);
    if (!cond)
        return 0;
    
    if (parser->_curr_token.type != TOKEN_THEN)
        return Error("expected then");
    advance_to_next_token(parser); // eat the then
    
    exp_node *then = _ParseExp(parser);
    if (!then)
        return 0;
    
    if (parser->_curr_token.type != TOKEN_ELSE)
        return Error("expected else");
    
    advance_to_next_token(parser);
    
    exp_node *else_exp = _ParseExp(parser);
    if (!else_exp)
        return 0;
    
    return (exp_node*)_createConditionNode(cond, then, else_exp);
}

/// unary
///   ::= primary
///   ::= '!' unary
exp_node *_ParseUnary(parser* parser) {
    // If the current token is not an operator, it must be a primary expr.
    if (!isascii(parser->_curr_token_num) || parser->_curr_token_num == '(' || parser->_curr_token_num == ','){
        //fprintf(stderr, "parse unary:%d\n", _curr_token_num);
        return _ParseNode(parser);
    }
    // If this is a unary operator, read it.
    int opc = parser->_curr_token.op_val;
    advance_to_next_token(parser);
    if (exp_node *operand = _ParseUnary(parser))
        return (exp_node*)_createUnaryNode(opc, operand);
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
exp_node* _ParseFor(parser* parser) {
    advance_to_next_token(parser); // eat the for.
    
    if (parser->_curr_token.type != TOKEN_IDENT)
        return Error("expected identifier after for");
    
    std::string id_name = *parser->_curr_token.ident_str;
    advance_to_next_token(parser); // eat identifier.
    
    if (parser->_curr_token.op_val != '=')
        return Error("expected '=' after for");
    advance_to_next_token(parser); // eat '='.
    
    exp_node *start = _ParseExp(parser);
    if (start == 0)
        return 0;
    if (parser->_curr_token.op_val != ',')
        return Error("expected ',' after for start value");
    advance_to_next_token(parser);
    
    exp_node *end = _ParseExp(parser);
    if (end == 0)
        return 0;
    
    // The step value is optional.
    exp_node *step = 0;
    if (parser->_curr_token.op_val == ',') {
        advance_to_next_token(parser);
        step = _ParseExp(parser);
        if (step == 0)
            return 0;
    }
    
    if (parser->_curr_token.type != TOKEN_IN)
        return Error("expected 'in' after for");
    advance_to_next_token(parser); // eat 'in'.
    
    exp_node* body = _ParseExp(parser);
    if (body == 0)
        return 0;
    
    return (exp_node*)_createForNode(id_name, start, end, step, body);
}

exp_node* _ParseIf(parser* parser) {
    advance_to_next_token(parser); // eat the if.
    
    // condition.
    exp_node *cond = _ParseExp(parser);
    if (!cond)
        return 0;
    
    if (parser->_curr_token.type != TOKEN_THEN)
        return Error("expected then");
    advance_to_next_token(parser); // eat the then
    
    exp_node *then = _ParseExp(parser);
    if (then == 0)
        return 0;
    
    if (parser->_curr_token.type != TOKEN_ELSE)
        return Error("expected else");
    
    advance_to_next_token(parser);
    
    exp_node * else_exp = _ParseExp(parser);
    if (!else_exp)
        return 0;
    
    return (exp_node*)_createConditionNode(cond, then, else_exp);
}
