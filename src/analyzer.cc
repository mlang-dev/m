#include "analyzer.h"


void _analyze_block(block_node* block){
  for(auto node: block->nodes){
    analyze(node);
  }
}

void _analyze_var(var_node* var){
  var->base.type = var->init_value->type;
}

void _analyze_bin(binary_node* bin){
  //bin->base.value_type = var->init_value->value_type;
}

void _analyze_una(unary_node* una){
  //bin->base.value_type = var->init_value->value_type;
}

void analyze(exp_node* node){
  if(node->node_type == BLOCK_NODE)
    _analyze_block((block_node*)node);
  else if(node->node_type==VAR_NODE)
    _analyze_var((var_node*)node);
  else if(node->node_type==UNARY_NODE)
    _analyze_una((unary_node*)node);
  else if(node->node_type==BINARY_NODE)
    _analyze_bin((binary_node*)node);
}