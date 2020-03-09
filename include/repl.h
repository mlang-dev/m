#include "jit.h"

JIT* build_jit(parser* parser);
void eval_statement(void* p_jit, exp_node* node);
double eval_exp(JIT* jit, exp_node* node);
int run_repl();