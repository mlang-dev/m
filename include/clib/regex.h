/*
 * struct regex.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file: a simple regex using Thompson method
 */
#ifndef __CLIB_REGEX_H__
#define __CLIB_REGEX_H__

#ifdef __cplusplus
extern "C" {
#endif

const char* to_postfix(const char *re_pattern);
void *regex_new(const char *re_pattern, const char *stop_chars);
int regex_match(void *re, const char *text);
void regex_free(void *re);

#ifdef __cplusplus
}
#endif

#endif
