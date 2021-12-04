/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to implement wasm module structure and its components
 * 
 */

#include "wasm/wasm.h"
#include "clib/array.h"

void _wasm_module_init(struct wasm_module *wm)
{
  array_init(&wm->types);
  array_init(&wm->functions);
  array_init(&wm->exports);
  array_init(&wm->codes);
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

