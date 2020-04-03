#include "dfa.h"

dfa *create_dfa()
{
    dfa *new_dfa = (dfa *)calloc(1, sizeof(dfa));
    new_dfa->num_states = 0;
    new_dfa->max_states = 1;
    new_dfa->states = (dfa_state **)calloc(1, sizeof(dfa_state *));
    return new_dfa;
}

void expand_dfa(dfa *d)
{
    assert(d->num_states == d->max_states, "dfa expansion called on correct condition");

    d->states = (dfa_state **)realloc((void *)(d->states), 2 * (d->max_states) * sizeof(dfa_state *));

    assert(d->states != NULL, "expansion of dfa successful");

    d->max_states = d->max_states * 2;
}

void expand_transition_space(dfa_state *s)
{
    assert(s->num_transitions == s->max_transitions, "transition expansion called on correct condition");

    s->transitions = (int **)realloc((void *)(s->transitions), 2 * (s->max_transitions) * sizeof(int *));

    assert(s->transitions != NULL, "expansion of transition space successful");

    s->max_transitions = s->max_transitions * 2;
}

void add_state(dfa *d, int id, bool final)
{
    assert(id == d->num_states, "nodes of dfa in spec file need to be in ascending order of ID (starting from 0)");

    if (d->num_states == d->max_states)
    {
        expand_dfa(d);
    }

    assert(d->num_states < d->max_states && d->num_states >= 0, "dfa ready for adding node");

    dfa_state *new_state = (dfa_state *)calloc(1, sizeof(dfa_state));
    new_state->id = id;
    new_state->final = final;
    new_state->num_transitions = 0;
    new_state->max_transitions = 1;
    new_state->transitions = (int **)calloc(1, sizeof(int *));
    new_state->default_transition = -1;

    d->states[id] = new_state;
    d->num_states = d->num_states + 1;
}

bool get_final(dfa_state *s)
{
    return s->final;
}

int get_state_id(dfa_state *s)
{
    return s->id;
}

dfa_state *get_start_state(dfa *d)
{
    assert(d->num_states > 0, "dfa is not empty (while fetching start state)");
    return d->states[0];
}

void add_transition(dfa *d, int from_state, int to_state, char trans_char)
{
    assert(from_state < d->num_states && to_state < d->num_states, "transition to be added has valid nodes");

    dfa_state *s = d->states[from_state];

    if (s->num_transitions == s->max_transitions)
    {
        expand_transition_space(s);
    }

    assert(s->num_transitions < s->max_transitions, "dfa state ready for transition addition");

    int *transition = calloc(3, sizeof(int));

    transition[0] = (int)trans_char;
    transition[1] = (int)trans_char;
    transition[2] = to_state;

    s->transitions[s->num_transitions] = transition;

    s->num_transitions = s->num_transitions + 1;
    transition = NULL;
}

void add_range_transition(dfa *d, int from_state, int to_state, char from_char, char to_char)
{
    assert(from_state < d->num_states && to_state < d->num_states, "range transition to be added has valid states");

    dfa_state *s = d->states[from_state];

    if (s->num_transitions == s->max_transitions)
    {
        expand_transition_space(s);
    }

    assert(s->num_transitions < s->max_transitions, "dfa state ready for range transition addition");

    int *transition = calloc(3, sizeof(int));

    transition[0] = (int)from_char;
    transition[1] = (int)to_char;
    transition[2] = to_state;

    s->transitions[s->num_transitions] = transition;

    s->num_transitions = s->num_transitions + 1;
    transition = NULL;
}

void add_default_transition(dfa *d, int from_state, int to_state)
{
    assert(from_state < d->num_states && to_state < d->num_states, "default transition being added has valid states");

    dfa_state *s = d->states[from_state];

    assert(s->default_transition == -1, "default transition isn't being overwritten");

    s->default_transition = to_state;
}

dfa_state *traverse_by_input(dfa *d, dfa_state *s, char input)
{
    int next_state_id = -1;

    for (int i = 0; i < s->num_transitions; i++)
    {
        if (((int)input) >= s->transitions[i][0] && ((int)input) <= s->transitions[i][1])
        {
            next_state_id = s->transitions[i][2];
            break;
        }
    }

    if (next_state_id == -1)
    {
        next_state_id = s->default_transition;
    }
    if (next_state_id == -1)
    {
        return NULL;
    }
    else
    {
        return d->states[next_state_id];
    }
}

void destroy_dfa_state(dfa_state *s)
{
    for (int i = 0; i < s->num_transitions; i++)
    {
        free(s->transitions[i]);
    }
    free(s->transitions);
    free(s);
}

void destroy_dfa(dfa *d)
{
    for (int i = 0; i < d->num_states; i++)
    {
        destroy_dfa_state(d->states[i]);
    }
    free(d->states);
    free(d);
}