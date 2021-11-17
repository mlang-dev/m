/*
 * wasm_codegen.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for WASM IR codegen
 */
#ifndef __MLANG_WASM_CODEGEN_H__
#define __MLANG_WASM_CODEGEN_H__

#include "parser/ast_node.h"

#ifdef __cplusplus
extern "C" {
#endif

void wasm_codegen_init();
string generate(struct ast_node *ast, const char *text);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__
