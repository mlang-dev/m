/*
 * amodule.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * one module is one translation unit
 */
#ifndef __MLANG_AMODULE_H__
#define __MLANG_AMODULE_H__

#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

struct amodule {
    struct ast_node *root_ast;
    const char *code;
};


#ifdef __cplusplus
}
#endif

#endif
