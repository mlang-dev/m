/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _menv{
    void* context;
}menv;

menv *env_new();
void env_free(menv* env);

#ifdef __cplusplus
}
#endif

#endif
