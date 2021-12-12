/*
 * recognizer.c
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * implementation for recognizer: the recognizer is the set of dfa state and their transitions based on
 * the input character
 */

#include "lexer/recognizer.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include <ctype.h>


void rcg_init(struct rcg_states *kss)
{
    for (size_t i = 0; i < ARRAY_SIZE(kss->states); i++) {
        kss->states[i] = 0;
    }
}

struct rcg_state *_rcg_state_new(char ch)
{
    struct rcg_state *ks;
    MALLOC(ks, sizeof(*ks));
    ks->accepted_token_or_opcode = TOKEN_NULL;
    ks->ch = ch;
    array_init(&ks->nexts, sizeof(struct rcg_state *));
    return ks;
}

void _rcg_state_free(struct rcg_state *ks)
{
    struct rcg_state *elemt = 0;
    for (size_t i = 0; i < array_size(&ks->nexts); i++) {
        elemt = *(struct rcg_state **)array_get(&ks->nexts, i);
        _rcg_state_free(elemt);
    }
    array_deinit(&ks->nexts);
    FREE(ks);
}

struct rcg_state *rcg_find_next_state(struct rcg_state *state, char ch)
{
    if (!state)
        return 0;
    struct rcg_state *ks = 0;
    for (size_t i = 0; i < array_size(&state->nexts); i++) {
        ks = *(struct rcg_state **)array_get(&state->nexts, i);
        if (ks->ch == ch)
            return ks;
    }
    return 0;
}

void rcg_add_string(struct rcg_states *rss, const char *str, enum token_type token_type)
{
    struct rcg_state *rs;
    struct rcg_state *next_rs;
    char ch = str[0];
    rs = rss->states[(int)ch];
    if (!rs) {
        rs = _rcg_state_new(ch);
        rss->states[(int)ch] = rs;
    }
    for (size_t j = 1; j < strlen(str); ++j) {
        ch = str[j];
        next_rs = rcg_find_next_state(rs, ch);
        if (!next_rs) {
            next_rs = _rcg_state_new(ch);
            array_push(&rs->nexts, &next_rs);
        }
        rs = next_rs;
    }
    rs->accepted_token_or_opcode = token_type;
    rs->identifiable = isalnum(str[0]);
}

void rcg_deinit(struct rcg_states *kss)
{
    for (size_t i = 0; i < ARRAY_SIZE(kss->states); i++) {
        if (kss->states[i]) {
            _rcg_state_free(kss->states[i]);
        }
    }
}
