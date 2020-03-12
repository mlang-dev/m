#include "astdump.h"
#include <sstream>

string to_string(vector<string> &array){
  ostringstream imploded;
  copy(array.begin(), array.end(),
           ostream_iterator<string>(imploded, " "));
  return imploded.str();
}

string _dump_prototype(prototype_node* proto){
  return proto->name + to_string(proto->args) + "=";
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
  return "var: " + var->var_name + "=" + dump(var->init_value);
}

string _dump_unary(unary_node* unary){
  return "un: " + unary->op + dump(unary->operand);
}

string _dump_binary(binary_node* binary){
  return "bin: " + dump(binary->lhs) + binary->op + dump(binary->rhs);
}

string _dump_call(call_node* call){
  vector<string> args;
  transform (call->args.begin(), call->args.end(), args.begin(), dump);
  return call->callee + " " + to_string(args);
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