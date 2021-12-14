/*
 * regex.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * simple regex implementation
 */
#include "clib/util.h"
#include "clib/regex.h"


const char* to_postfix(const char *re)
{
	int nalt, natom;
	static char buf[8000];
	char *dst;
	struct {
		int nalt;
		int natom;
	} paren[100], *p;
	
	p = paren;
	dst = buf;
	nalt = 0;
	natom = 0;
	if(strlen(re) >= sizeof buf/2)
		return NULL;
	for(; *re; re++){
		switch(*re){
		case '(':
			if(natom > 1){
				--natom;
				*dst++ = '.';
			}
			if(p >= paren+100)
				return NULL;
			p->nalt = nalt;
			p->natom = natom;
			p++;
			nalt = 0;
			natom = 0;
			break;
		case '|':
			if(natom == 0)
				return NULL;
			while(--natom > 0)
				*dst++ = '.';
			nalt++;
			break;
		case ')':
			if(p == paren)
				return NULL;
			if(natom == 0)
				return NULL;
			while(--natom > 0)
				*dst++ = '.';
			for(; nalt > 0; nalt--)
				*dst++ = '|';
			--p;
			nalt = p->nalt;
			natom = p->natom;
			natom++;
			break;
		case '*':
		case '+':
		case '?':
			if(natom == 0)
				return NULL;
			*dst++ = *re;
			break;
		default:
			if(natom > 1){
				--natom;
				*dst++ = '.';
			}
			*dst++ = *re;
			natom++;
			break;
		}
	}
	if(p != paren)
		return NULL;
	while(--natom > 0)
		*dst++ = '.';
	for(; nalt > 0; nalt--)
		*dst++ = '|';
	*dst = 0;
	return buf;
}

struct nstate *nstate_new(int op, struct nstate *out1, struct nstate *out2)
{
    struct nstate *s;
    MALLOC(s, sizeof(*s));
    s->op = op;
    s->out1 = out1;
    s->out2 = out2;
    return s;
}

union list_ptr *to_list(struct nstate **out)
{
    union list_ptr *p = (union list_ptr *)out;
    p->next = 0;
    return p;
}

union list_ptr *join(union list_ptr *p1, union list_ptr *p2)
{
    union list_ptr *p = p1;
    while(p->next) p = p->next;
    p->next = p2;
    return p1;
}

void connect(union list_ptr *out, struct nstate *in)
{
    union list_ptr *next;
    for(; out; out = next){
        next = out->next;
        out->state = in;
    }
}

struct nfa _nfa(struct nstate *in, union list_ptr *out)
{
    struct nfa a = {in, out};
    return a;
}

struct nstate accepted_state = {NS_ACCEPT, 0, 0};

struct nstate *to_nfa(const char *pattern)
{
    const char *p;
    struct nfa stack[1024];
    struct nfa *sp = stack;
    struct nfa a1, a2, a;
    struct nstate *s;
    if(!pattern) return 0;
    for(p=pattern; *p; p++){
        switch(*p){
            default:
                s = nstate_new(*p, 0, 0);
               *sp++ = _nfa(s, to_list(&s->out1)); //push to the stack
                break;
            case '.':
                a2 = *--sp;
                a1 = *--sp;
                connect(a1.out, a2.in);
                *sp++ = _nfa(a1.in, a2.out); //push to the stack
                break;
            case '|':
                a2 = *--sp;
                a1 = *--sp;
                s = nstate_new(NS_SPLIT, a1.in, a2.in);
                *sp++ = _nfa(s, join(a1.out, a2.out));
                break;
            case '?':
                a = *--sp;
                s = nstate_new(NS_SPLIT, a.in, 0);
                *sp++ = _nfa(s, join(a.out, to_list(&s->out2)));
                break;
            case '*':
                a = *--sp;
                s = nstate_new(NS_SPLIT, a.in, 0);
                connect(a.out, s);
                *sp++ = _nfa(s, to_list(&s->out2));
                break;
            case '+':
                a = *--sp;
                s = nstate_new(NS_SPLIT, a.in, 0);
                connect(a.out, s);
                *sp++ = _nfa(a.in, to_list(&s->out2));
                break;
        }
    }
    a = *--sp;
    if(sp != stack) return 0;
    connect(a.out, &accepted_state);
    return a.in;
}

