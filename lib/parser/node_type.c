/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * defines all ast's node types and strings
 *
 */
#include "parser/node_type.h"
#include "lexer/terminal.h"
#include "clib/array.h"
#include "parser/grammar.h"
#include "clib/string.h"
#include <assert.h>

#define NODE_TYPE_NAME(node_name, node_type) {#node_name, 0, node_type##_NODE}

struct node_type_name node_type_names[TERMINAL_COUNT] = {
    NODE_TYPE_NAME(null, NULL),
    NODE_TYPE_NAME(unit, UNIT),
    NODE_TYPE_NAME(import_decl, IMPORT),
    NODE_TYPE_NAME(memory_decl, MEMORY),
    NODE_TYPE_NAME(literal, LITERAL),
    NODE_TYPE_NAME(ident, IDENT),
    NODE_TYPE_NAME(var, VAR),
    NODE_TYPE_NAME(enum_def, ENUM),
    NODE_TYPE_NAME(cast, CAST),
    NODE_TYPE_NAME(struct_def, STRUCT),
    NODE_TYPE_NAME(union_def, UNION),
    NODE_TYPE_NAME(struct_init, STRUCT_INIT),
    NODE_TYPE_NAME(array_init, ARRAY_INIT),
    NODE_TYPE_NAME(array_type, ARRAY_TYPE),
    NODE_TYPE_NAME(range, RANGE),
    NODE_TYPE_NAME(unop, UNARY),
    NODE_TYPE_NAME(binop, BINARY),
    NODE_TYPE_NAME(indexing, MEMBER_INDEX),
    NODE_TYPE_NAME(if_cond, IF),
    NODE_TYPE_NAME(for_loop, FOR),
    NODE_TYPE_NAME(while_loop, WHILE),
    NODE_TYPE_NAME(jump, JUMP),
    NODE_TYPE_NAME(call, CALL),
    NODE_TYPE_NAME(func_type, FUNC_TYPE),
    NODE_TYPE_NAME(func, FUNC),
    NODE_TYPE_NAME(block, BLOCK),
    NODE_TYPE_NAME(total, TOTAL)
};

const char *node_type_strings[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

struct hashtable node_type_names_by_symbol;

struct node_type_name *get_node_type_names()
{
    return node_type_names;
}

void node_type_init()
{
    hashtable_init(&node_type_names_by_symbol);
    for(int i = 0; i < TOTAL_NODE; i++){
        struct node_type_name *ntn = &node_type_names[i];
        ntn->symbol_name = to_symbol2(ntn->name, strlen(ntn->name));
        hashtable_set_p(&node_type_names_by_symbol, ntn->symbol_name, ntn);
    }
}

void node_type_deinit()
{
    hashtable_deinit(&node_type_names_by_symbol);
}

struct node_type_name *get_node_type_name_by_symbol(symbol symbol)
{
    return (struct node_type_name *)hashtable_get_p(&node_type_names_by_symbol, symbol);
}

enum node_type symbol_to_node_type(symbol action_name)
{
    if(!action_name)
        return NULL_NODE;
    struct node_type_name *ntn = get_node_type_name_by_symbol(action_name);
    if(!ntn) return 0;
    return ntn->node_type;
}

