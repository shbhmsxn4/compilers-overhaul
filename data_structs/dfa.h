/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdbool.h>
#include <stdlib.h>
#include "../utils/gen_utils.h"

#ifndef DFA_H
#define DFA_H

typedef struct ____DFA_STATE____ dfa_state;
typedef struct ____DFA____ dfa;

struct ____DFA_STATE____
{
    int id;
    bool final;
    int num_transitions;
    int max_transitions;
    int **transitions;
    int default_transition; // -1 if not valid
};

struct ____DFA____
{
    dfa_state **states;
    int num_states;
    int max_states;
};

dfa *create_dfa();

void add_state(dfa *d, int id, bool final);

bool get_final(dfa_state *n);

int get_state_id(dfa_state *n);

dfa_state *get_start_state(dfa *d);

void add_transition(dfa *d, int from_state, int to_state, char trans_char);

void add_range_transition(dfa *d, int from_state, int to_state, char from_char, char to_char);

void add_default_transition(dfa *d, int from_state, int to_state);

// returns NULL if no valid transition
dfa_state *traverse_by_input(dfa *d, dfa_state *s, char input);

void destroy_dfa(dfa *d);

#endif