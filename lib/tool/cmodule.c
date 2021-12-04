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
#include "parser/m_parser.h"
#include <clang-c/Index.h>
#include <stdio.h>

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

struct func_type_node *create_function_func_type(CXCursor cursor)
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

        symbol var_name = to_symbol(clang_getCString(cx_arg_name));
        clang_disposeString(cx_arg_name);
        if (!string_size(var_name)) {
            string format = str_format("arg%d", i);
            var_name = to_symbol(string_get(&format));
        }
        symbol annotated_type_name = get_type_symbol(arg_type);
        struct source_location param_loc = {0, 0, 0, 0};
        struct ast_node *fun_param = var_node_new(0, param_loc, var_name, 0, annotated_type_name, 0);
        fun_param->type = (struct type_exp *)create_nullary_type(arg_type, annotated_type_name);
        array_push(&fun_params, &fun_param);
    }
    struct source_location loc = { 0, 1, 0, 0 };
    return func_type_node_default_new(0, loc, string_2_symbol(&fun_name), &fun_params, ret_type, is_variadic, true);
}

enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    (void)parent;
    (void)client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);

    // Consider functions and methods
    if (kind == CXCursor_FunctionDecl || kind == CXCursor_CXXMethod) {
        struct func_type_node *node = create_function_func_type(cursor);
        if (node) {
            struct array *arr = (struct array *)client_data;
            array_push(arr, &node);
        }
    }
    return CXChildVisit_Recurse;
}

struct array parse_c_file(const char *file_path)
{
    struct array func_types;
    array_init(&func_types, sizeof(struct func_type_node *));
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        file_path, 0, 0,
        0, 0,
        CXTranslationUnit_None);
    if (unit == 0) {
        printf("Unable to parse translation unit for %s. Quitting.\n", file_path);
        return func_types;
    }
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(
        cursor,
        cursor_visitor,
        &func_types);
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return func_types;
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
