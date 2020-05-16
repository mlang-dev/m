/*
 * compiler.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for parse c file
 */
#ifndef __MLANG_CMODULE_H__
#define __MLANG_CMODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

struct array parse_c_file(const char* fn);

#ifdef __cplusplus
}
#endif

#endif
