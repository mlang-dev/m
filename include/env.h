/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENVIRONMENT_H__
#define __MLANG_ENVIRONMENT_H__

struct menv{
    void* context;
};

menv *env_new();
void env_free(menv* env);

#endif