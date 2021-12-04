/*
 * ast_node.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic ast header file
 */
#ifndef __MLANG_AST_NODE_H__
#define __MLANG_AST_NODE_H__

#include <stdio.h>

#include "clib/symbol.h"
#include "clib/array.h"
#include "sema/type.h"
#include "parser/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
struct ast_node {
    symbol node_type;
    symbol type_name;
    struct type_exp *type; // type inferred
    struct source_location loc;
    struct array children; //list of pointer to child ast_node
};

struct ast_node *ast_node_new(symbol node_type);
void ast_node_free(struct ast_node *node);

struct ast_context{
    struct ast_node *parent;
};

typedef void (*visit)(struct ast_node *node);

void ast_node_walk(struct ast_context *c, struct ast_node *node);
#ifdef __cplusplus
}
#endif

#endif
