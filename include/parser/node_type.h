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
#include "sema/type.h"
#include "lexer/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
#define FOREACH_NODETYPE(ENUM_ITEM) \
    ENUM_ITEM(NULL_NODE)            \
    ENUM_ITEM(UNIT_NODE)            \
    ENUM_ITEM(IMPORT_NODE)          \
    ENUM_ITEM(MEMORY_NODE)          \
    ENUM_ITEM(LITERAL_NODE)         \
    ENUM_ITEM(IDENT_NODE)           \
    ENUM_ITEM(VAR_NODE)             \
    ENUM_ITEM(ENUM_NODE)            \
    ENUM_ITEM(STRUCT_NODE)          \
    ENUM_ITEM(UNION_NODE)           \
    ENUM_ITEM(STRUCT_INIT_NODE)     \
    ENUM_ITEM(LIST_COMP_NODE)       \
    ENUM_ITEM(RANGE_NODE)           \
    ENUM_ITEM(UNARY_NODE)           \
    ENUM_ITEM(BINARY_NODE)          \
    ENUM_ITEM(MEMBER_INDEX_NODE)    \
    ENUM_ITEM(IF_NODE)              \
    ENUM_ITEM(FOR_NODE)             \
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

void node_type_init();
void node_type_deinit();

#ifdef __cplusplus
}
#endif

#endif

