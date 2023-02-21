/*
 * node_type.h
 *
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * ast header file for ast node type
 */
#ifndef __MLANG_NODE_TYPE_H__
#define __MLANG_NODE_TYPE_H__

#include <stdio.h>

#include "clib/symbol.h"
#include "clib/util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
#define FOREACH_NODETYPE(ENUM_ITEM) \
    ENUM_ITEM(NULL_NODE)            \
    ENUM_ITEM(IMPORT_NODE)          \
    ENUM_ITEM(MEMORY_NODE)          \
    ENUM_ITEM(LITERAL_NODE)         \
    ENUM_ITEM(IDENT_NODE)           \
    ENUM_ITEM(VAR_NODE)             \
    ENUM_ITEM(CAST_NODE)            \
    ENUM_ITEM(RECORD_NODE)          \
    ENUM_ITEM(VARIANT_NODE)           \
    ENUM_ITEM(VARIANT_TYPE_ITEM_NODE) \
    ENUM_ITEM(ADT_INIT_NODE)        \
    ENUM_ITEM(ARRAY_INIT_NODE)      \
    ENUM_ITEM(ARRAY_TYPE_NODE)      \
    ENUM_ITEM(TYPE_EXPR_ITEM_NODE)  \
    ENUM_ITEM(TYPE_ITEM_NODE)       \
    ENUM_ITEM(TYPE_NODE)            \
    ENUM_ITEM(RANGE_NODE)           \
    ENUM_ITEM(UNARY_NODE)           \
    ENUM_ITEM(BINARY_NODE)          \
    ENUM_ITEM(ASSIGN_NODE)          \
    ENUM_ITEM(MEMBER_INDEX_NODE)    \
    ENUM_ITEM(IF_NODE)              \
    ENUM_ITEM(MATCH_NODE)           \
    ENUM_ITEM(MATCH_CASE_NODE)      \
    ENUM_ITEM(WILDCARD_NODE)        \
    ENUM_ITEM(FOR_NODE)             \
    ENUM_ITEM(WHILE_NODE)           \
    ENUM_ITEM(JUMP_NODE)            \
    ENUM_ITEM(CALL_NODE)            \
    ENUM_ITEM(FUNC_TYPE_NODE)       \
    ENUM_ITEM(FUNC_NODE)            \
    ENUM_ITEM(BLOCK_NODE)           \
    ENUM_ITEM(TOTAL_NODE)

enum node_type { FOREACH_NODETYPE(GENERATE_ENUM) };

struct node_type_name{
    const char *name;       //c string name
    symbol symbol_name;     //symbol name
    enum node_type node_type; //node type enum
};

extern const char *node_type_strings[];

struct node_type_name *get_node_type_name_by_symbol(symbol symbol);

struct node_type_name *get_node_type_names();

enum node_type symbol_to_node_type(symbol action_name);

void node_type_init();
void node_type_deinit();

#ifdef __cplusplus
}
#endif

#endif

