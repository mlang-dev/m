#include "lexer/keyword.h"
#include "clib/util.h"
#include <ctype.h>


void kss_init(struct keyword_states *kss)
{
    for (size_t i = 0; i < ARRAY_SIZE(kss->states); i++) {
        kss->states[i] = 0;
    }
}

struct keyword_state *_new_keyword_state(char ch)
{
    struct keyword_state *ks;
    MALLOC(ks, sizeof(*ks));
    ks->accept = false;
    ks->ch = ch;
    array_init(&ks->nexts, sizeof(struct keyword_state *));
    return ks;
}

void _free_keyword_state(struct keyword_state *ks)
{
    struct keyword_state *elemt = 0;
    for (size_t i = 0; i < array_size(&ks->nexts); i++) {
        elemt = *(struct keyword_state **)array_get(&ks->nexts, i);
        _free_keyword_state(elemt);
    }
    array_deinit(&ks->nexts);
    FREE(ks);
}

struct keyword_state *find_next_keyword_state(struct keyword_state *state, char ch)
{
    if (!state)
        return 0;
    struct keyword_state *ks = 0;
    for (size_t i = 0; i < array_size(&state->nexts); i++) {
        ks = *(struct keyword_state **)array_get(&state->nexts, i);
        if (ks->ch == ch)
            return ks;
    }
    return 0;
}

void kss_add_string(struct keyword_states *kss, const char *str)
{
    struct keyword_state *ks;
    struct keyword_state *next_ks;
    char ch = str[0];
    ks = kss->states[(int)ch];
    if (!ks) {
        ks = _new_keyword_state(ch);
        kss->states[(int)ch] = ks;
    }
    for (size_t j = 1; j < strlen(str); ++j) {
        ch = str[j];
        next_ks = find_next_keyword_state(ks, ch);
        if (!next_ks) {
            next_ks = _new_keyword_state(ch);
            array_push(&ks->nexts, &next_ks);
        }
        ks = next_ks;
    }
    ks->accept = true;
    ks->identifiable = isalnum(str[0]);
}

void kss_deinit(struct keyword_states *kss)
{
    for (size_t i = 0; i < ARRAY_SIZE(kss->states); i++) {
        if (kss->states[i]) {
            _free_keyword_state(kss->states[i]);
        }
    }
}
