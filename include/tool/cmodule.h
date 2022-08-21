/*
 * cmodule.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for parse c file
 */
#ifndef __MLANG_CMODULE_H__
#define __MLANG_CMODULE_H__

#include "clib/array.h"

#ifdef __cplusplus
extern "C" {
#endif

struct array parse_c_file(const char* file_path);
bool transpile_2_m(const char* head, const char* mfile);

#ifdef __cplusplus
}
#endif

#endif
