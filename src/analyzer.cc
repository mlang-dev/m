#include "analyzer.h"


type_exp* retrieve(type_env* env, string name){
  return retrieve(name, env->nogens, env->type_env);
}

type_exp* _analyze_ident(type_env* env, ident_node* ident){
  return retrieve(env, ident->name);
}

type_exp* _analyze_num(type_env* env, num_node* num){
  return retrieve(env, TypeString[num->base.type]);
}

type_exp* _analyze_var(type_env* env, var_node* var){
  type_exp* result_type = (type_exp*)create_type_var();
  auto value_type = analyze(env, var->init_value);
  unify(result_type, value_type, env->nogens);
  return result_type;
}

type_exp* _analyze_call(type_env* env, call_node* call){
  auto fun_type = retrieve(env, call->callee);
  vector<type_exp*> args; args.resize(call->args.size());
  transform(call->args.begin(), call->args.end(), args.begin(), 
  [&](exp_node* node){
    return analyze(env, node);
  });
  type_exp* result_type = (type_exp*)create_type_var();
  unify((type_exp*)create_type_fun(args, result_type), fun_type, env->nogens);
  return result_type;
}

type_exp* _analyze_fun(type_env* env, function_node* fun){
  //# create a new non-generic variable for the binder
  vector<type_exp*> args; args.resize(fun->prototype->args.size());
  transform(fun->prototype->args.begin(), fun->prototype->args.end(), args.begin(), 
  [](string arg){
    return (type_exp*)create_type_var();
  });
  for(int i = 0; i<args.size(); i++){
    env->nogens.push_back(args[i]);
    env->type_env[fun->prototype->args[i]] = args[i];
  }
  type_exp* result_type = analyze(env, fun->body->nodes.back()); //TODO: need to recursively analyze
  return (type_exp*)create_type_fun(args, result_type);
}

type_exp* _analyze_bin(type_env* env, binary_node* bin){
  //bin->base.value_type = var->init_value->value_type;
  return nullptr;
}

type_exp* _analyze_una(type_env* env, unary_node* una){
  //bin->base.value_type = var->init_value->value_type;
  return nullptr;
}

type_exp* _analyze_if(type_env* env, condition_node* cond){
  return nullptr;
}

type_exp* analyze(type_env* env, exp_node* node){
  if(node->node_type == IDENT_NODE)
    return _analyze_ident(env, (ident_node*)node);
  if(node->node_type == NUMBER_NODE)
    return _analyze_num(env, (num_node*)node);
  else if(node->node_type==VAR_NODE)
    return _analyze_var(env, (var_node*)node);
  else if(node->node_type==UNARY_NODE)
    return _analyze_una(env, (unary_node*)node);
  else if(node->node_type==BINARY_NODE)
    return _analyze_bin(env, (binary_node*)node);
  else if(node->node_type == CALL_NODE)
    return _analyze_call(env, (call_node*)node);
  else if(node->node_type == FUNCTION_NODE)
    return _analyze_fun(env, (function_node*)node);
  else if(node->node_type == CONDITION_NODE)
    return _analyze_if(env, (condition_node*)node);
  return nullptr;
}

type_env* create_type_env(){
  type_env* env = new type_env();
  vector<type_exp*> args;
  for(auto def_type: TypeString)
    env->type_env[def_type] = (type_exp*)create_type_oper(def_type, args);
  return env;
}

void destroy_type_env(type_env* env){
  for(auto type: env->type_env){
    destroy_type_exp(type.second);
  }
  delete env;
}

vector<type_exp*> analyze(type_env* env, block_node* block){
  vector<type_exp*> exps;
  for(auto node: block->nodes){
    exps.push_back(analyze(env, node));
  }
  return exps;
}
