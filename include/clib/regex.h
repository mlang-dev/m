/*
 * struct regex.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file: a simple regex using Thompson method
 */
#ifndef __CLIB_REGEX_H__
#define __CLIB_REGEX_H__

#define NS_SPLIT 256
#define NS_ACCEPT 257


#ifdef __cplusplus
extern "C" {
#endif

struct nstate{
    int op; //character if op < 256
    struct nstate *out1;
    struct nstate *out2;
};

union list_ptr{
    union list_ptr *next;
    struct nstate *state;
};

struct nfa{
    struct nstate *in;
    union list_ptr *out;
};

const char* to_postfix(const char *re);

#ifdef __cplusplus
}
#endif

#endif
