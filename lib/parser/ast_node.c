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

struct ast_node *ast_node_new(symbol node_type)
{
    struct ast_node *node;
    MALLOC(node, sizeof(*node));
    node->node_type = node_type;
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

string print(struct ast_node *ast, const char *text)
{
    string s;
    string_init(&s);
    if(!ast){
        return s;
    }
    if(ast->node_type){
        string itself;
        string_init_chars2(&itself, &text[ast->loc.start], ast->loc.end - ast->loc.start);
        string_add(&s, &itself);
    }
    size_t child_count = array_size(&ast->children);
    for(size_t i = 0; i < child_count; i++){
        struct ast_node *child = *(struct ast_node**)array_get(&ast->children, i);
        string child_s = print(child, text);
        if(string_size(&s))
            string_add_chars2(&s, " ", 1);
        string_add(&s, &child_s);
    }
    return s;
}