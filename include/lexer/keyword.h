/*
 * keyword.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for keyword string
 */
#ifndef __MLANG_KEYWORD_H__
#define __MLANG_KEYWORD_H__
#include "clib/array.h"

#ifdef __cplusplus
extern "C" {
#endif
struct keyword_state {
    char ch;
    int accepted_token_type;
    bool identifiable;
    struct array nexts;
};

struct keyword_states {
    struct keyword_state *states[256];
};

void kss_init(struct keyword_states *kss);
void kss_add_string(struct keyword_states *kss, const char *str, enum token_type token_type);
void kss_deinit(struct keyword_states *kss);
struct keyword_state *find_next_keyword_state(struct keyword_state *state, char ch);

#ifdef __cplusplus
}
#endif

#endif
