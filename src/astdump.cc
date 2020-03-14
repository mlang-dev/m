#include "astdump.h"
#include "util.h"
#include <sstream>


string _dump_prototype(prototype_node* proto){
  return proto->name + vector_to_string(proto->args) + "=";
}

string _dump_block(block_node* block){
  string block_string = "";
  for(auto node: block->nodes){
    block_string += "  " + dump(node) + "\n";
  }
  return block_string;
}

string _dump_function(function_node* func){
  string stmt = _dump_prototype(func->prototype) + "\n";
  return stmt + _dump_block(func->body);
}

string _dump_var(var_node* var){
  string vars = "var: ";
  vars += var->var_name + "=" + dump(var->init_value);
  return vars; 
}

string _dump_unary(unary_node* unary){
  string un = "un: ";
  un += unary->op + dump(unary->operand);
  return un;
}

string _dump_binary(binary_node* binary){
  string bin = "bin: ";
  bin += dump(binary->lhs) + binary->op + dump(binary->rhs);
  return bin;
}

string _dump_call(call_node* call){
  vector<string> args;
  transform (call->args.begin(), call->args.end(), args.begin(), dump);
  return call->callee + " " + vector_to_string(args);
}

string _dump_if(condition_node* cond){
  auto stmt = "if " + dump(cond->condition_node) + " then " + dump(cond->then_node);
  if(cond->else_node)
    stmt += " else " + dump(cond->else_node);
  return stmt;
}

string _dump_for(for_node* fornode){
  return "for " + fornode->var_name + " in " + dump(fornode->start) + ".." + dump(fornode->end);
}

string _dump_id(ident_node* idnode){
  return "id: " + idnode->name;
}

string _dump_number(num_node* node){
  return "num: " + to_string(node->num_val);
}

string dump(exp_node* node){
  if(node->type==FUNCTION_NODE)
    return _dump_function((function_node*)node);
  else if(node->type == PROTOTYPE_NODE)
    return _dump_prototype((prototype_node*)node);
  else if(node->type == VAR_NODE)
    return _dump_var((var_node*)node);
  else if(node->type == UNARY_NODE)
    return _dump_unary((unary_node*)node);
  else if(node->type == BINARY_NODE)
    return _dump_binary((binary_node*)node);
  else if(node->type == CONDITION_NODE)
    return _dump_if((condition_node*)node);
  else if(node->type == CALL_NODE)
    return _dump_call((call_node*)node);
  else if(node->type == FOR_NODE)
    return _dump_for((for_node*)node);
  else if(node->type == IDENT_NODE)
    return _dump_id((ident_node*)node);
  else if(node->type == NUMBER_NODE)
    return _dump_number((num_node*)node);
  else{
    string not_supported = "ast->type not supported: ";
    return not_supported + NodeTypeString[node->type];
  }
}