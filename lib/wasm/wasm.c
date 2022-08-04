/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement wasm module structure and its components
 * 
 */

#include "wasm/wasm.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "codegen/wasm_codegen.h"
#include "lexer/init.h"
#include "parser/amodule.h"
#include "parser/m_grammar.h"
#include "parser/lalr_parser.h"

void _wasm_module_init(struct wasm_module *wm)
{
  array_init(&wm->types, sizeof(void*));
  array_init(&wm->functions, sizeof(void*));
  array_init(&wm->exports, sizeof(void*));
  array_init(&wm->codes, sizeof(void*));
}

void _wasm_module_deinit(struct wasm_module *wm)
{
  array_deinit(&wm->codes);
  array_deinit(&wm->exports);
  array_deinit(&wm->functions);
  array_deinit(&wm->types);
}

struct wasm_module *wasm_module_new()
{
  struct wasm_module *wm;
  MALLOC(wm, sizeof(*wm));
  _wasm_module_init(wm);
  return wm;
}

void wasm_module_free(struct wasm_module *wm)
{
  _wasm_module_deinit(wm);
  free(wm);
}

const char *fun_def = "run()=";

struct byte_array parse_exp_as_module(const char *expr)
{
    frontend_init();
    wasm_codegen_init();
    string m_code;
    string_init_chars2(&m_code, fun_def, strlen(fun_def));
    string_add_chars2(&m_code, expr, strlen(expr));
    struct lalr_parser *parser = parser_new();
    const char *m_text = string_get(&m_code);
    struct ast_node *ast = parse_text(parser, m_text);
    struct amodule mod;
    mod.root_ast = ast;
    mod.code = m_text;
    struct byte_array ba = wasm_emit(mod);
    string_deinit(&m_code);
    lalr_parser_free(parser);
    frontend_deinit();
    return ba;
}
