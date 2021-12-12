/*
 * grammar.h
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for grammar of m
 */
#ifndef __MLANG_M_GRAMMAR_H__
#define __MLANG_M_GRAMMAR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct keyword_token{
    const char *keyword;
    union{
        enum token_type token_type;
        enum op_code opcode;
    };
};

extern struct keyword_token keyword_tokens[];
extern int keyword_count;

const char * get_m_grammar();
const char * get_opcode(int opcode);

#ifdef __cplusplus
}
#endif

#endif
