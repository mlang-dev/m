/*
 * repl.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m repl
 */
#ifndef __MLANG_REPL_H__
#define __MLANG_REPL_H__

#include "compiler/jit.h"
#include "compiler/engine.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct eval_result {
    enum type type;
    union {
        double d_value;
        int i_value;
        bool b_value;
        char c_value;
        const char *s_value;
    };
};

struct JIT *build_jit(struct engine *engine);
//analyze first then evaluate the node
void eval(void *p_jit, struct ast_node *node);
void eval_statement(void *p_jit, struct ast_node *node);
struct eval_result eval_exp(struct JIT *jit, struct ast_node *node);
int run_repl();

#ifdef __cplusplus
}
#endif

#endif
