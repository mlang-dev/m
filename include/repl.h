/*
 * repl.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m repl
 */
#ifndef __MLANG_REPL_H__
#define __MLANG_REPL_H__

#include "env.h"
#include "jit.h"

#ifdef __cplusplus
extern "C"{
#endif

JIT* build_jit(menv* env, parser* parser);
void eval_statement(void* p_jit, exp_node* node);
double eval_exp(JIT* jit, exp_node* node);
int run_repl();

#ifdef __cplusplus
}
#endif

#endif