/*
 * runtime.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m runtime
 */
#ifndef __MLANG_RUNTIME_H__
#define __MLANG_RUNTIME_H__
#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT int print(int x);

#ifdef __cplusplus
}
#endif

#endif
