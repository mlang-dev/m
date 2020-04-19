/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * AST print functions
 */
#include "astdump.h"
#include "clib/util.h"
#include <sstream>

std::string _join(std::vector<std::string> args)
{
    array arr;
    string_array_init(&arr);
    string str_arg;
    for(auto arg:args){
        string_init_chars(&str_arg, arg.c_str());
        array_push(&arr, &str_arg);
    }
    string str = string_join(&arr, ' ');
    std::string result = std::string(str.data);
    string_deinit(&str);
    string_deinit(&str_arg);
    array_deinit(&arr);
    return result;
}

std::string _dump_block(block_node* node)
{
    std::string block = "blk: \n";
    for (auto stmt : node->nodes)
        block += "  " + dump(stmt) + "\n";
    return block;
}

std::string _dump_prototype(prototype_node* proto)
{
    return std::string(proto->name.data) + _join(proto->args) + "=";
}

std::string _dump_function(function_node* func)
{
    std::string stmt = _dump_prototype(func->prototype) + "\n";
    return stmt + _dump_block(func->body);
}

std::string _dump_var(var_node* var)
{
    std::string vars = "var: ";
    std::string varname(var->var_name.data);
    vars += varname + "=" + dump(var->init_value);
    return vars;
}

std::string _dump_unary(unary_node* unary)
{
    std::string un = "un: ";
    std::string op(unary->op.data);
    un += op + dump(unary->operand);
    return un;
}

std::string _dump_binary(binary_node* binary)
{
    std::string bin = "bin:";
    bin += std::string(binary->op.data);
    bin += "[";
    bin += dump(binary->lhs) + "," + dump(binary->rhs);
    bin += "]";
    return bin;
}

std::string _dump_call(call_node* call)
{
    std::vector<std::string> args;
    transform(call->args.begin(), call->args.end(), args.begin(), dump);
    return std::string(call->callee.data) + " " + _join(args);
}

std::string _dump_if(condition_node* cond)
{
    auto stmt = "if " + dump(cond->condition_node) + " then " + dump(cond->then_node);
    if (cond->else_node)
        stmt += " else " + dump(cond->else_node);
    return stmt;
}

std::string _dump_for(for_node* fornode)
{
    return "for " + std::string(fornode->var_name.data) + " in " + dump(fornode->start) + ".." + dump(fornode->end);
}

std::string _dump_id(ident_node* idnode)
{
    std::string idname(idnode->name.data);
    return "id: " + idname;
}

std::string _dump_number(num_node* node)
{
    return "num: " + std::to_string(node->double_val);
}

std::string dump(exp_node* node)
{
    if (node->node_type == FUNCTION_NODE)
        return _dump_function((function_node*)node);
    else if (node->node_type == PROTOTYPE_NODE)
        return _dump_prototype((prototype_node*)node);
    else if (node->node_type == VAR_NODE)
        return _dump_var((var_node*)node);
    else if (node->node_type == UNARY_NODE)
        return _dump_unary((unary_node*)node);
    else if (node->node_type == BINARY_NODE)
        return _dump_binary((binary_node*)node);
    else if (node->node_type == CONDITION_NODE)
        return _dump_if((condition_node*)node);
    else if (node->node_type == CALL_NODE)
        return _dump_call((call_node*)node);
    else if (node->node_type == FOR_NODE)
        return _dump_for((for_node*)node);
    else if (node->node_type == IDENT_NODE)
        return _dump_id((ident_node*)node);
    else if (node->node_type == NUMBER_NODE)
        return _dump_number((num_node*)node);
    else if (node->node_type == BLOCK_NODE)
        return _dump_block((block_node*)node);
    else {
        std::string not_supported = "ast->node_type not supported: ";
        return not_supported + NodeTypeString[node->node_type];
    }
}