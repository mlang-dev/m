/*
 * recognizer.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for recognizer: the recognizer is the set of dfa state and their transitions based on
 * the input character
 */
#ifndef __MLANG_KEYWORD_H__
#define __MLANG_KEYWORD_H__
#include "clib/array.h"

#ifdef __cplusplus
extern "C" {
#endif
struct rcg_state {
    char ch;
    int accepted_token_or_opcode;
    bool identifiable;
    struct array nexts;
};

struct rcg_states {
    struct rcg_state *states[256];
};

void rcg_init(struct rcg_states *rss);
void rcg_add_string(struct rcg_states *rss, const char *str, enum token_type token_type);
void rcg_deinit(struct rcg_states *rss);
struct rcg_state *rcg_find_next_state(struct rcg_state *state, char ch);

#ifdef __cplusplus
}
#endif

#endif
