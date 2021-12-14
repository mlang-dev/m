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

const char* to_postfix(const char *re_pattern);
void *regex_new(const char *re_pattern);
int regex_match(void *re, const char *text);
void regex_free(void *re);

#ifdef __cplusplus
}
#endif

#endif
