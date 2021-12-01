/*
 * wasm_codegen.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for WASM IR Binary Format codegen
 */
#ifndef __MLANG_WASM_CODEGEN_H__
#define __MLANG_WASM_CODEGEN_H__

#include "parser/ast_node.h"
#include "clib/byte_array.h"

#ifdef __cplusplus
extern "C" {
#endif

void wasm_codegen_init();
struct byte_array wasm_emit(struct ast_node *ast, const char *code);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_WASM_CODEGEN_H__
