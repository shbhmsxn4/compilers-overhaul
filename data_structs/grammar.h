#include <stdlib.h>
#include "../lang_specs/entities.h"
#include "../utils/gen_utils.h"

#ifndef GRAMMAR_H
#define GRAMMAR_H

struct ____GRAMMAR_RULE____
{
    nonterminal lhs;
    int rhs_len;
    gm_unit *rhs;
};
typedef struct ____GRAMMAR_RULE____ gm_rule;

struct ____GRAMMAR____
{
    nonterminal start_symbol;
    gm_rule *rules;
    int num_rules;
    int max_rules;
};

typedef struct ____GRAMMAR____ grammar;

void set_start_symbol(grammar *gm, nonterminal nt);

grammar *create_grammar();

void set_start_symbol(grammar *gm, nonterminal nt);

void add_rule(grammar *gm, nonterminal lhs, gm_unit *rhs, int rhs_len);

gm_first *get_first(grammar *gm);

gm_follow *get_follow(grammar *gm, gm_first *gmfs);

#endif

/*

Functions to implement:

get first

get follow

create parse table

*/