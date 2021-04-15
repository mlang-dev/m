/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * cmodule implementation: using libclang to parse c header file
 * 
 */
#include "clib/array.h"
#include "clib/string.h"
#include "parser/ast.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>

enum type _get_type(CXType cxtype)
{
    enum type type = TYPE_UNK;
    if (cxtype.kind == CXType_Double) {
        type = TYPE_DOUBLE;
    } else if (cxtype.kind == CXType_Bool) {
        type = TYPE_BOOL;
    } else if (cxtype.kind == CXType_Int || cxtype.kind == CXType_UInt) {
        type = TYPE_INT;
    } else if (cxtype.kind == CXType_Pointer) {
        type = TYPE_STRING;
    } else if (cxtype.kind == CXType_Char_S) {
        type = TYPE_CHAR;
    }
    return type;
}

struct prototype_node *create_function_prototype(CXCursor cursor)
{
    CXType cur_type = clang_getCursorType(cursor);

    CXString cx_fun_name = clang_getCursorSpelling(cursor);
    string fun_name;
    string_init_chars(&fun_name, clang_getCString(cx_fun_name));
    clang_disposeString(cx_fun_name);
    CXType cx_type = clang_getResultType(cur_type);
    enum type type = _get_type(cx_type);
    if (!type) {
        return 0;
    }
    struct type_exp *ret_type = (struct type_exp *)create_nullary_type(type, get_type_symbol(type));
    ARRAY_FUN_PARAM(fun_params);
    struct var_node fun_param;
    fun_param.base.annotated_type = 0;
    fun_param.base.type = 0;
    fun_param.base.node_type = VAR_NODE;
    int num_args = clang_Cursor_getNumArguments(cursor);
    bool is_variadic = clang_isFunctionTypeVariadic(cur_type);
    for (int i = 0; i < num_args; ++i) {
        CXType cx_arg_type = clang_getArgType(cur_type, i);
        enum type arg_type = _get_type(cx_arg_type);
        if (!arg_type) {
            return 0;
        }
        CXCursor arg_cursor = clang_Cursor_getArgument(cursor, i);
        CXString cx_arg_name = clang_getCursorSpelling(arg_cursor);
        fun_param.var_name = to_symbol(clang_getCString(cx_arg_name));
        clang_disposeString(cx_arg_name);
        if (!string_size(fun_param.var_name)) {
            string format = str_format("arg%d", i);
            fun_param.var_name = to_symbol(string_get(&format));
        }
        fun_param.base.annotated_type_name = get_type_symbol(arg_type);
        fun_param.base.annotated_type = 0;
        fun_param.base.type = (struct type_exp *)create_nullary_type(arg_type, fun_param.base.annotated_type_name);
        array_push(&fun_params, &fun_param);
    }
    struct source_loc loc = { 0, 1 };
    return prototype_node_default_new(0, loc, string_2_symbol(&fun_name), &fun_params, ret_type, is_variadic, true);
}

enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    (void)parent;
    (void)client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);

    // Consider functions and methods
    if (kind == CXCursor_FunctionDecl || kind == CXCursor_CXXMethod) {
        struct prototype_node *node = create_function_prototype(cursor);
        if (node) {
            struct array *arr = (struct array *)client_data;
            array_push(arr, &node);
        }
    }
    return CXChildVisit_Recurse;
}

struct array parse_c_file(const char *file_path)
{
    struct array prototypes;
    array_init(&prototypes, sizeof(struct prototype_node *));
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        file_path, 0, 0,
        0, 0,
        CXTranslationUnit_None);
    if (unit == 0) {
        printf("Unable to parse translation unit for %s. Quitting.\n", file_path);
        return prototypes;
    }
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(
        cursor,
        cursor_visitor,
        &prototypes);
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return prototypes;
}

void _write_to_file(struct array *codes, const char *mfile)
{
    FILE *fp;
    fp = fopen(mfile, "w");
    for (size_t i = 0; i < array_size(codes); i++) {
        fprintf(fp, "%s\n", string_get(array_get(codes, i)));
    }
    fclose(fp);
}

bool transpile_2_m(const char *head, const char *mfile)
{
    struct array protos = parse_c_file(head);
    ARRAY_STRING(codes);
    for (size_t i = 0; i < array_size(&protos); i++) {
        struct exp_node *node = *(struct exp_node **)array_get(&protos, i);
        string code = dump(node);
        array_push(&codes, &code);
    }
    _write_to_file(&codes, mfile);
    return true;
}