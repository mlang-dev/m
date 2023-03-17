/*
 * ld.h
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * interface header file invoking ld linker
 */
#ifndef __COMPILER_LD_H__
#define __COMPILER_LD_H__

#ifdef __cplusplus
extern "C" {
#endif

int ld(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif