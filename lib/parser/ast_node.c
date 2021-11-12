/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language ast node code implementation.
 *
 */
#include "clib/array.h"
#include "clib/util.h"
#include "parser/ast_node.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct ast_node *ast_node_new()
{
    struct ast_node *node;
    MALLOC(node, sizeof(*node));
    node->node_type = 0;
    node->type_name = 0;
    node->type = 0;
    node->loc.start = 0;
    node->loc.end = 0;
    node->loc.row = 0;
    node->loc.col = 0;
    array_init(&node->children, sizeof(struct ast_node *));
    return node;
}

void ast_node_free(struct ast_node *node)
{
    if(!node) return;
    size_t len = array_size(&node->children);
    for(size_t i = 0; i < len; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->children, i);
        ast_node_free(child);
    }
    array_deinit(&node->children);
    free(node);
}

string print(struct ast_node *ast)
{
    (void)ast;
    string s;
    string_init(&s);
    return s;
}