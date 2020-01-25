#include <map>
#include <memory>
#include "parser.h"
#include "util.h"

int _get_op_precedence(parser* parser);
exp_node* _parse_number(parser* parser);
exp_node* _parse_parentheses(parser* parser);
exp_node* _parse_ident(parser* parser);
exp_node* _parse_node(parser* parser);
exp_node* _parse_binary(parser* parser, int exp_prec, exp_node* lhs);
exp_node* _parse_exp(parser* parser);
exp_node* _parse_condition(parser* parser);
exp_node* _parse_for(parser* parser);
exp_node* _parse_if(parser* parser);
exp_node* _parse_unary(parser* parser);
exp_node* _parse_var(parser* parser);
exp_node* _parse_prototype(parser* parser);


function_node* _create_function_node(prototype_node* prototype, exp_node* body){
    auto node = new function_node();
    node->base.type = FUNCTION_NODE;
    node->prototype = prototype;
    node->body = body;
    return node;
}

ident_node* _create_ident_node(std::string& name){
    auto node = new ident_node();
    node->base.type = IDENT_NODE;
    node->name = name;
    return node;
}

num_node* _create_num_node(double val){
    auto node = new num_node();
    node->base.type = NUMBER_NODE;
    node->num_val = val;
    return node;
}

var_node* _create_var_node(const std::vector<std::pair<std::string, exp_node*>> &var_names,
               exp_node* body){
    auto node = new var_node();
    node->base.type = VAR_NODE;
    node->body = body;
    node->var_names = var_names;
    return node;
}

call_node* _create_call_node(const std::string &callee,
                std::vector<exp_node*> &args){
    auto node = new call_node();
    node->base.type = CALL_NODE;
    node->callee = callee;
    node->args = args;
    return node;
}

prototype_node* _create_prototype_node(const std::string &name, std::vector<std::string> &args,
                  bool is_operator = false, unsigned precedence = 0, bool is_a_value = false){
    auto node = new prototype_node();
    node->base.type = PROTOTYPE_NODE;
    node->name = name;
    node->args = args;
    node->is_operator = is_operator;
    node->precedence = precedence;
    node->is_a_value = is_a_value;
    return node;
}

condition_node* _create_condition_node(exp_node* condition, exp_node* then_node, exp_node* else_node){
    auto node = new condition_node();
    node->base.type = CONDITION_NODE;
    node->condition_node = condition;
    node->then_node = then_node;
    node->else_node = else_node;
    return node;
}

unary_node* _create_unary_node(char op, exp_node* operand){
    auto node = new unary_node();
    node->base.type = UNARY_NODE;
    node->op = op;
    node->operand = operand;
    return node;
}

binary_node* _create_binary_node(char op, exp_node* lhs, exp_node* rhs){
    auto node = new binary_node();
    node->base.type = BINARY_NODE;
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

for_node* _create_for_node(const std::string &var_name, exp_node* start, exp_node* end, exp_node* step, exp_node* body){
    auto node = new for_node();
    node->base.type = FOR_NODE;
    node->var_name = var_name;
    node->start = start;
    node->end = end;
    node->step = step;
    node->body = body;
    return node;
}

std::map<char, int> g_op_precedences = {
    {'<', 10},
    {'+', 20},
    {'-', 20},
    {'*', 40},
    {'/', 40}
};

parser* create_parser(bool create_entry){
    auto psr = new parser();
    psr->op_precedences=&g_op_precedences;
    psr->ast = new ast();
    if(create_entry){
        psr->ast->entry_module = new module();
        psr->ast->modules.push_back(psr->ast->entry_module);
    }
    return psr;
}

void destroy_parser(parser* parser){
    std::vector<module*>::iterator begin = parser->ast->modules.begin();
    std::vector<module*>::iterator end = parser->ast->modules.end();
    std::vector<module*>::iterator it;
    for (it = begin; it != end; ++it){
        delete *it;
    delete parser->ast;
    delete parser;
}

int parse_next_token(parser* parser){
    auto token = get_token();
    parser->curr_token = token;
    parser->curr_token_num = token.type == TOKEN_OP? token.op_val : token.type;
    //fprintf(stderr, "got token: %d, %f, %d\n", parser->curr_token.type, 
    //    parser->curr_token.num_val, parser->curr_token_num);
    return parser->curr_token_num;
}

int _get_op_precedence(parser* parser){
    if(!isascii(parser->curr_token_num))
        return -1;
    int op_precedence = (*parser->op_precedences)[parser->curr_token_num];
    //fprintf(stderr, "op %d: pre: %d\n", op, op_precedence);
    if (op_precedence <= 0)
        return -1;
    return op_precedence;
}

exp_node* _parse_number(parser* parser){
    auto result = _create_num_node(parser->curr_token.num_val);
    parse_next_token(parser);
    return (exp_node*)result;
}

exp_node* _parse_parentheses(parser* parser){
    parse_next_token(parser);
    auto v = _parse_exp(parser);
    if(!v)
        return 0;
    if(parser->curr_token.op_val!=')')
        return (exp_node*)log(ERROR, "expected ')'");
    parse_next_token(parser);
    return v;
}

exp_node* _parse_ident(parser* parser){
    std::string id_name = *parser->curr_token.ident_str;
    parse_next_token(parser); //take identifier
    if (parser->curr_token_num!='(') {//pure variable
        //fprintf(stderr, "ident parsed. %s\n", id_name.c_str());
        return (exp_node*)_create_ident_node(id_name);
    }
    parse_next_token(parser);//take next
    std::vector<exp_node*> args;
    if (parser->curr_token_num != ')'){
        while(1){
            if(auto arg = _parse_exp(parser))
                args.push_back(arg);
            else
                return 0;
            if (parser->curr_token_num==')')
                break;
            
            if (parser->curr_token_num != ',')
                return (exp_node*)log(ERROR, "Expected ')' or ',' in argument list");
            parse_next_token(parser);
        }
    }
    parse_next_token(parser);
    return (exp_node*)_create_call_node(id_name, args);
}

/// varexpr ::= 'let' identifier ('=' expression)?
//                    (',' identifier ('=' expression)?)* 'in' expression
exp_node* _parse_var(parser* parser) {
    parse_next_token(parser); // eat the let.
    // At least one variable name is required.
    if (parser->curr_token.type != TOKEN_IDENT)
        return (exp_node*)log(ERROR, "expected identifier after let");
    std::vector<std::pair<std::string, exp_node *> > var_names;
    while (1) {
        std::string name = *parser->curr_token.ident_str;
        parse_next_token(parser); // eat identifier.
        
        // Read the optional initializer.
        exp_node *init = 0;
        if (parser->curr_token_num == '=') {
            parse_next_token(parser); // eat the '='.
            init = _parse_exp(parser);
            if (init == 0)
                return 0;
        }
        
        var_names.push_back(std::make_pair(name, init));
        // End of let list, exit loop.
        if (parser->curr_token_num != ',')
            break;
        parse_next_token(parser); // eat the ','.
        
        if (parser->curr_token.type != TOKEN_IDENT)
            return (exp_node*)log(ERROR, "expected identifier list after var");
    }    
    // At this point, we have to have 'in'.
    if (parser->curr_token.type != TOKEN_IN)
        return (exp_node*)log(ERROR, "expected 'in' keyword after 'var'");
    parse_next_token(parser); // eat 'in'.
    exp_node *body = _parse_exp(parser);
    if (body == 0)
        return 0;
    return (exp_node*)_create_var_node(var_names, body);
}

exp_node* _parse_node(parser* parser){
    if(parser->curr_token.type == TOKEN_IDENT)
        return _parse_ident(parser);
    else if(parser->curr_token.type == TOKEN_NUM)
        return _parse_number(parser);
    else if(parser->curr_token.type == TOKEN_IF)
        return _parse_if(parser);
    else if(parser->curr_token.type == TOKEN_FOR)
        return _parse_for(parser);
    else if(parser->curr_token.type == TOKEN_VAR)
        return _parse_var(parser);
    else if(parser->curr_token_num == '(')
        return _parse_parentheses(parser);
    else{
        std::string error = "unknown token: " + std::to_string(parser->curr_token.type);
        if (parser->curr_token.type == TOKEN_OP)
            error += " op: " + std::to_string(parser->curr_token.op_val);
        return (exp_node*)log(ERROR, error.c_str());
    }
}

exp_node* _parse_binary(parser* parser, int exp_prec, exp_node* lhs){
    while(true){
        int tok_prec = _get_op_precedence(parser);
        //fprintf(stderr, "op prec1: %d, %d\n", tok_prec, exp_prec);
        if (tok_prec < exp_prec)
            return lhs;
        
        int binary_op = parser->curr_token_num;
        parse_next_token(parser);
        auto rhs = _parse_unary(parser); //_parse_node
        if (!rhs)
            return lhs;
        
        auto next_prec = _get_op_precedence(parser);
        //fprintf(stderr, "op prec2: %d, %d\n", tok_prec, next_prec);
        if (tok_prec < next_prec){
            rhs = _parse_binary(parser, tok_prec + 1, rhs);
            if (!rhs)
                return 0;
        }
        lhs = (exp_node*)_create_binary_node(binary_op, lhs, rhs);
    }
}

exp_node* _parse_exp(parser* parser){
    auto lhs = _parse_unary(parser); //_parse_node
    if (!lhs)
        return 0;
    return _parse_binary(parser, 0, lhs);
}

/// prototype
///   ::= id '(' id* ')'
///   ::= binary LETTER number? (id, id)
///   ::= unary LETTER (id)
exp_node* _parse_prototype(parser* parser) {
    std::string fun_name;
    //SourceLocation FnLoc = CurLoc;
    unsigned proto_type = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned bin_prec = 30;
    int token = 0;
    switch (parser->curr_token.type) {
        case TOKEN_IDENT:
            fun_name = *parser->curr_token.ident_str;
            proto_type = 0;
            //fprintf(stderr, "ident token in parse prototype: %s\n", fun_name.c_str());
            parse_next_token(parser);
            break;
        case TOKEN_UNARY:
            token = parse_next_token(parser);
            if (!isascii(token))
                return (exp_node*)log(ERROR, "Expected unary operator");
            fun_name = "unary";
            fun_name += (char)token;
            proto_type = 1;
            parse_next_token(parser);
            break;
        case TOKEN_BINARY:
            token = parse_next_token(parser);
            if (!isascii(token))
                return (exp_node*)log(ERROR, "Expected binary operator");
            fun_name = "binary";
            fun_name += (char)token;
            proto_type = 2;
            parse_next_token(parser);        
            // Read the precedence if present.
            if (parser->curr_token.type == TOKEN_NUM) {
                if (parser->curr_token.num_val < 1 || parser->curr_token.num_val > 100)
                    return (exp_node*)log(ERROR, "Invalid precedecnce: must be 1..100");
                bin_prec = (unsigned)parser->curr_token.num_val;
                parse_next_token(parser);
            }
            break;
        default:
            return (exp_node*)log(ERROR, "Expected function name in prototype");
    }
    auto has_parenthese = parser->curr_token_num == '(';
    if (has_parenthese)
        parse_next_token(parser);
    std::vector<std::string> arg_names;
    while (parser->curr_token_num == TOKEN_IDENT){
        //fprintf(stderr, "arg names: %s", (*parser->curr_token.ident_str).c_str());
        arg_names.push_back(*parser->curr_token.ident_str);
        parse_next_token(parser);
    }
    if (has_parenthese && parser->curr_token_num != ')')
        return (exp_node*)log(ERROR, "Expected ')' to match '('");
    // success.
    if (has_parenthese)
        parse_next_token(parser); // eat ')'.
    // Verify right number of names for operator.
    if (proto_type && arg_names.size() != proto_type)
        return (exp_node*)log(ERROR, "Invalid number of operands for operator");
    //is a value: has no parenthese and no parameters
    auto is_a_value = !has_parenthese && arg_names.size() == 0;
    //fprintf(stderr, "creating prototype: %s, args: %lu, current token: %c\n", fun_name.c_str(), arg_names.size(), parser->curr_token_num);
    return (exp_node*)_create_prototype_node(fun_name, arg_names, proto_type != 0, bin_prec, is_a_value);
}

exp_node* parse_function(parser* parser){
    parse_next_token(parser);
    auto prototype = (prototype_node*)_parse_prototype(parser);
    if (!prototype)
       return 0;
    if (parser->curr_token_num == '=')
        parse_next_token(parser); //skip '=' token
    if (auto exp = _parse_exp(parser)){
        if(0){ //(prototype->is_a_value){
            //not a function but a value
            std::vector<std::pair<std::string, exp_node *> > var_names;
            var_names.push_back(std::make_pair(prototype->name, exp));
            log(INFO, "parse_function: converting to a value: %d!", exp->type);
            return (exp_node*)_create_var_node(var_names, exp);
        }
        else{        
            return (exp_node*)_create_function_node((prototype_node*)prototype, exp);
        }
    }
    return 0;
}

exp_node* parse_exp_to_function(parser* parser){
    if (auto e = _parse_exp(parser)){
        auto args = std::vector<std::string>();
        auto prototype = _create_prototype_node("main", args);
        return (exp_node*)_create_function_node(prototype, e);
    }
    return 0;
}

exp_node* parse_import(parser* parser){
    parse_next_token(parser);
    return _parse_prototype(parser);
}

/// ifexpr ::= 'if' expression 'then' expression 'else' expression
exp_node* _parse_condition(parser* parser) {
    parse_next_token(parser); // eat the if.
    
    // condition.
    exp_node *cond = _parse_exp(parser);
    if (!cond)
        return 0;
    
    if (parser->curr_token.type != TOKEN_THEN)
        return (exp_node*)log(ERROR, "expected then");
    parse_next_token(parser); // eat the then
    
    exp_node *then = _parse_exp(parser);
    if (!then)
        return 0;
    
    if (parser->curr_token.type != TOKEN_ELSE)
        return (exp_node*)log(ERROR, "expected else");
    
    parse_next_token(parser);
    
    exp_node *else_exp = _parse_exp(parser);
    if (!else_exp)
        return 0;
    
    return (exp_node*)_create_condition_node(cond, then, else_exp);
}

/// unary
///   ::= primary
///   ::= '!' unary
exp_node *_parse_unary(parser* parser) {
    // If the current token is not an operator, it must be a primary expr.
    if (!isascii(parser->curr_token_num) || parser->curr_token_num == '(' 
        || parser->curr_token_num == ','){
        //fprintf(stderr, "parse unary:%f\n", parser->curr_token.num_val);
        return _parse_node(parser);
    }
    //fprintf(stderr, "unary: %d, %d\n", parser->curr_token.op_val, parser->curr_token_num);
    // If this is a unary operator, read it.
    int opc = parser->curr_token.op_val;
    parse_next_token(parser);
    if (exp_node *operand = _parse_unary(parser))
        return (exp_node*)_create_unary_node(opc, operand);
    return 0;
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
exp_node* _parse_for(parser* parser) {
    parse_next_token(parser); // eat the for.
    
    if (parser->curr_token.type != TOKEN_IDENT)
        return (exp_node*)log(ERROR, "expected identifier after for");
    
    std::string id_name = *parser->curr_token.ident_str;
    parse_next_token(parser); // eat identifier.
    
    if (parser->curr_token.op_val != '=')
        return (exp_node*)log(ERROR, "expected '=' after for");
    parse_next_token(parser); // eat '='.
    
    exp_node *start = _parse_exp(parser);
    if (start == 0)
        return 0;
    if (parser->curr_token.op_val != ',')
        return (exp_node*)log(ERROR, "expected ',' after for start value");
    parse_next_token(parser);
    
    exp_node *end = _parse_exp(parser);
    if (end == 0)
        return 0;
    
    // The step value is optional.
    exp_node *step = 0;
    if (parser->curr_token.op_val == ',') {
        parse_next_token(parser);
        step = _parse_exp(parser);
        if (step == 0)
            return 0;
    }
    
    if (parser->curr_token.type != TOKEN_IN)
        return (exp_node*)log(ERROR, "expected 'in' after for");
    parse_next_token(parser); // eat 'in'.
    
    exp_node* body = _parse_exp(parser);
    if (body == 0)
        return 0;
    
    return (exp_node*)_create_for_node(id_name, start, end, step, body);
}

exp_node* _parse_if(parser* parser) {
    parse_next_token(parser); // eat the if.
    
    // condition.
    exp_node *cond = _parse_exp(parser);
    if (!cond)
        return 0;
    
    if (parser->curr_token.type != TOKEN_THEN)
        return (exp_node*)log(ERROR, "expected then");
    parse_next_token(parser); // eat the then
    
    exp_node *then = _parse_exp(parser);
    if (then == 0)
        return 0;
    
    if (parser->curr_token.type != TOKEN_ELSE)
        return (exp_node*)log(ERROR, "expected else");
    
    parse_next_token(parser);
    
    exp_node * else_exp = _parse_exp(parser);
    if (!else_exp)
        return 0;
    
    return (exp_node*)_create_condition_node(cond, then, else_exp);
}
