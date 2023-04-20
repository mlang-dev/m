/*
 * frontend.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m front end initializer
 */
#ifndef __MLANG_FRONTEND_H__
#define __MLANG_FRONTEND_H__

#include "parser/parser.h"
#include "sema/sema_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct frontend{
    struct parser *parser;
    struct sema_context *sema_context;
};

struct frontend *frontend_llvm_init(const char *sys_path, bool is_repl);
struct frontend *frontend_init();
void frontend_deinit(struct frontend *fe);

#ifdef __cplusplus
}
#endif

#endif
