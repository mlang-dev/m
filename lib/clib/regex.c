/*
 * regex.c
 *
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * simple regex implementation
 */
#include "clib/util.h"
#include "clib/regex.h"
#include "clib/list.h"

#define RE_MAX_PAREN 100
#define RE_CONCAT '.'


const char* to_postfix(const char *re)
{
	int nalt, natom, ncharset;
	static char buf[8000];
	char *dst;
	struct {
		int nalt;
		int natom;
        int ncharset;
	} paren[RE_MAX_PAREN], *p;
	
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
        if(*re == '\\'){
            *dst++ = *re;
            continue;
        }
        else if(re > orig && *(re-1) == '\\'){
            *dst++ = *re;
            natom++;
            if(natom > 1){
                --natom;
                *dst++ = RE_CONCAT;
            }
            continue;
        }
		switch(*re){
		case '(':
			if(natom > 1){
				--natom;
				*dst++ = RE_CONCAT;
			}
			if(p >= paren+RE_MAX_PAREN)
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
				*dst++ = RE_CONCAT;
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
				*dst++ = RE_CONCAT;
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
                    *dst++ = RE_CONCAT;
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
		*dst++ = RE_CONCAT;
	for(; nalt > 0; nalt--)
		*dst++ = '|';
	*dst = 0;
	return buf;
}

#define NS_SPLIT 256
#define NS_ACCEPT 257

struct nstate{
    int op; //character if op < 256
    struct nstate *out1;
    struct nstate *out2;
    int last_listid;
};

union list_ptr{
    union list_ptr *next;
    struct nstate *state;
};

struct nfa{
    struct nstate *in;
    union list_ptr *out;
};

struct nstate_list{
    struct nstate **states;
    int len;
};

struct nstate_link_entry {
    list_entry(nstate_link_entry) list;
    struct nstate *state;
};

list_head(nstate_link_list, nstate_link_entry);


struct re{
    struct nstate_link_list states;
    int nstate_count;
    struct nstate_list l1, l2;
    int listid;
    struct nstate *start;
    struct nstate accepted_state;
};

void _ll_add_to_head(struct nstate_link_list *ll, struct nstate *state)
{
    struct nstate_link_entry *entry;
    MALLOC(entry, sizeof(*entry));
    entry->state = state;
    entry->list.next = 0;
    list_insert_head(ll, entry, list);
}

struct nstate *_ll_remove_from_head(struct nstate_link_list *ll)
{
    struct nstate *state = 0;
    if (!ll) {
        return 0;
    }
    if (ll->first) {
        struct nstate_link_entry *first = ll->first;
        state = first->state;
        list_remove_head(ll, list);
        FREE(first);
    }
    return state;
}

struct nstate *nstate_new(struct re *re, int op, struct nstate *out1, struct nstate *out2)
{
    struct nstate *s;
    MALLOC(s, sizeof(*s));
    s->op = op;
    s->out1 = out1;
    s->out2 = out2;
    s->last_listid = 0;
    _add_to_head(&re->states, s);
    re->nstate_count++;
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

struct nstate *to_nfa(struct re *re, const char *pattern)
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
                s = nstate_new(re, *p, 0, 0);
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
                s = nstate_new(re, NS_SPLIT, a1.in, a2.in);
                *sp++ = _nfa(s, join(a1.out, a2.out));
                break;
            case '?':
                a = *--sp;
                s = nstate_new(re, NS_SPLIT, a.in, 0);
                *sp++ = _nfa(s, join(a.out, to_list(&s->out2)));
                break;
            case '*':
                a = *--sp;
                s = nstate_new(re, NS_SPLIT, a.in, 0);
                connect(a.out, s);
                *sp++ = _nfa(s, to_list(&s->out2));
                break;
            case '+':
                a = *--sp;
                s = nstate_new(re, NS_SPLIT, a.in, 0);
                connect(a.out, s);
                *sp++ = _nfa(a.in, to_list(&s->out2));
                break;
        }
    }
    a = *--sp;
    if(sp != stack) return 0;
    connect(a.out, &re->accepted_state);
    return a.in;
}

void _nstates_add_state(struct re* re, struct nstate_list *l, struct nstate *s)
{
	if(s == NULL || s->last_listid == re->listid)
		return;
	s->last_listid = re->listid;
	if(s->op == NS_SPLIT){
		/* follow unlabeled arrows */
		_nstates_add_state(re, l, s->out1);
		_nstates_add_state(re, l, s->out2);
		return;
	}
	l->states[l->len++] = s;
}

struct nstate_list *_nstates_init(struct re *re, struct nstate_list *l, struct nstate *start)
{
	l->len = 0;
	re->listid++;
	_nstates_add_state(re, l, start);
	return l;
}

void _nstates_step(struct re *re, struct nstate_list *clist, int c, struct nstate_list *nlist)
{
	int i;
	struct nstate *s;

	re->listid++;
	nlist->len = 0;
	for(i=0; i<clist->len; i++){
		s = clist->states[i];
		if(s->op == c)
			_nstates_add_state(re, nlist, s->out1);
	}
}

int _nstates_is_accepted(struct re *re, struct nstate_list *l)
{
	int i;
	for(i=0; i<l->len; i++)
		if(l->states[i] == &re->accepted_state)
			return 1;
	return 0;
}

int _nstate_match(struct re *re, const char *text)
{
	int i, c;
	struct nstate_list *clist, *nlist, *t;
	clist = _nstates_init(re, &re->l1, re->start);
	nlist = &re->l2;
    const char *p = text;
	for(; *p; p++){
		c = *p & 0xFF;
		_nstates_step(re, clist, c, nlist);
		t = clist; clist = nlist; nlist = t;	/* swap clist, nlist */
	}
	return _nstates_is_accepted(re, clist);
}

void *regex_new(const char *re_pattern)
{
    struct re *re;
    MALLOC(re, sizeof(*re));
    re->states.first = 0;
    re->nstate_count = 0;
    re->listid = 0;
    re->start = 0;
    const char *re_postfix = to_postfix(re_pattern);
    if(!re_postfix) return 0;
    re->start = to_nfa(re, re_postfix);
	MALLOC(re->l1.states, re->nstate_count*sizeof re->l1.states[0]);
	MALLOC(re->l2.states, re->nstate_count*sizeof re->l2.states[0]);
    return re;
}

int regex_match(void *re, const char *text)
{
    return _nstate_match((struct re*)re, text);
}

void regex_free(void *regex)
{   
    struct re *re = (struct re *)regex;
    FREE(re->l1.states);
    FREE(re->l2.states);
    struct nstate *s;
    while(s = _ll_remove_from_head(&re->states)){
        FREE(s);
    }
    FREE(re);
}