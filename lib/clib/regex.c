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
    #define MAX_PAREN 100
    #define CONCAT '.'
	int nalt, natom, ncharset;
	static char buf[8000];
	char *dst;
	struct {
		int nalt;
		int natom;
        int ncharset;
	} paren[MAX_PAREN], *p;
	
	p = paren;
	dst = buf;
	nalt = 0;
	natom = 0;
    ncharset = 0;
    int nalt_charset = 0;
	if(strlen(re) >= sizeof buf/2)
		return 0;
    const char *orig = re;
	for(; *re; re++){
		switch(*re){
		case '(':
			if(natom > 1){
				--natom;
				*dst++ = CONCAT;
			}
			if(p >= paren+MAX_PAREN)
				return 0;
			p->nalt = nalt;
			p->natom = natom;
            p->ncharset = ncharset;
			p++;
			nalt = 0;
			natom = 0;
            ncharset = 0;
			break;
		case '|':
			if(natom == 0)
				return 0;
			while(--natom > 0)
				*dst++ = CONCAT;
			nalt++;
			break;
        case '[':
            nalt_charset = nalt;
            ncharset++;
            break;
        case ']':
            /*for each char, we add 1 to nalt, we need to reduce by 1*/
            nalt--;
            /*convert to alternation*/
			for(; nalt > nalt_charset; nalt--)
				*dst++ = '|';
            ncharset--;
            natom++;
            nalt_charset = 0;
            break;
		case ')':
			if(p == paren)
				return 0;
			if(natom == 0)
				return 0;
			while(--natom > 0)
				*dst++ = CONCAT;
			for(; nalt > 0; nalt--)
				*dst++ = '|';
			--p;
			nalt = p->nalt;
			natom = p->natom;
            ncharset = p->ncharset;
			natom++;
			break;
		case '*':
		case '+':
		case '?':
			if(natom == 0)
				return 0;
			*dst++ = *re;
			break;
		default:
            if(ncharset > 0){
                if(ncharset > 1) return 0;
                if(*re != '-'){
                    /*add missed range chars*/
                    if(*(re-1) == '-'){
                        if(re-2 < orig) return 0;
                        char from = *(re-2);
                        if (from > *re) return 0;
                        for(char c = from + 1; c <= *re; c++){
                            *dst++ = c;
                            nalt++;
                        }
                    }else{
                        *dst++ = *re;
                        nalt++;
                    }
                }
            }else{
                if(natom > 1){
                    --natom;
                    *dst++ = CONCAT;
                }
                *dst++ = *re;
                natom++;
            }
			break;
		}
	}
	if(p != paren)
		return 0;
	while(--natom > 0)
		*dst++ = CONCAT;
	for(; nalt > 0; nalt--)
		*dst++ = '|';
	*dst = 0;
	return buf;
}

struct nstate{
    int op; //character if op < 256
    struct nstate *out1;
    struct nstate *out2;
    int lastlist;
};

union list_ptr{
    union list_ptr *next;
    struct nstate *state;
};

struct nfa{
    struct nstate *in;
    union list_ptr *out;
};

struct nstate accepted_state = {NS_ACCEPT, 0, 0};
int nstate_count;

struct nstate *nstate_new(int op, struct nstate *out1, struct nstate *out2)
{
    struct nstate *s;
    MALLOC(s, sizeof(*s));
    s->op = op;
    s->out1 = out1;
    s->out2 = out2;
    s->lastlist = 0;
    nstate_count++;
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

struct nstate_list{
    struct nstate **s;
    int len;
};

struct nstate_list l1, l2;
static int listid;

void _nstates_add_state(struct nstate_list *l, struct nstate *s)
{
	if(s == NULL || s->lastlist == listid)
		return;
	s->lastlist = listid;
	if(s->op == NS_SPLIT){
		/* follow unlabeled arrows */
		_nstates_add_state(l, s->out1);
		_nstates_add_state(l, s->out2);
		return;
	}
	l->s[l->len++] = s;
}

struct nstate_list *_nstates_init(struct nstate *start, struct nstate_list *l)
{
	l->len = 0;
	listid++;
	_nstates_add_state(l, start);
	return l;
}

void _nstates_step(struct nstate_list *clist, int c, struct nstate_list *nlist)
{
	int i;
	struct nstate *s;

	listid++;
	nlist->len = 0;
	for(i=0; i<clist->len; i++){
		s = clist->s[i];
		if(s->op == c)
			_nstates_add_state(nlist, s->out1);
	}
}

int _nstates_is_accepted(struct nstate_list *l)
{
	int i;
	for(i=0; i<l->len; i++)
		if(l->s[i] == &accepted_state)
			return 1;
	return 0;
}

int _nstate_match(struct nstate *start, const char *s)
{
	int i, c;
	struct nstate_list *clist, *nlist, *t;

	clist = _nstates_init(start, &l1);
	nlist = &l2;
	for(; *s; s++){
		c = *s & 0xFF;
		_nstates_step(clist, c, nlist);
		t = clist; clist = nlist; nlist = t;	/* swap clist, nlist */
	}
	return _nstates_is_accepted(clist);
}

void *regex_new(const char *re_pattern)
{
    nstate_count = 0;
    const char *re_postfix = to_postfix(re_pattern);
    if(!re_postfix) return 0;
    struct nstate *s = to_nfa(re_postfix);
	l1.s = malloc(nstate_count*sizeof l1.s[0]);
	l2.s = malloc(nstate_count*sizeof l2.s[0]);
    return s;
}

int regex_match(void *re, const char *text)
{
    return _nstate_match((struct nstate*)re, text);
}

void regex_free(void *re)
{

}