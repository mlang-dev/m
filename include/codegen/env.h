/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "clib/array.h"
#include "clib/hashset.h"
#include "clib/symboltable.h"
#include "codegen/codegen.h"
#include "parser/m_parser.h"
#include "sema/sema_context.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    struct m_parser *parser;
    struct code_generator *cg;
    struct sema_context *sema_context;
};

struct env *env_new(bool is_repl);
void env_free(struct env *env);
struct env *get_env();
struct type_exp *emit_code(struct env *env, struct exp_node *node);

#define is_int_type(type) (type == TYPE_INT || type == TYPE_BOOL || type == TYPE_CHAR)

#ifdef __cplusplus
}
#endif

#endif
