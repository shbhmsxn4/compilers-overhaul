/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "parse_dfa.h"

dfa *parse_dfa(char *dfa_spec_filepath)
{
    FILE *dfa_spec_fptr = fopen(dfa_spec_filepath, "r");
    assert(dfa_spec_fptr != NULL, "dfa spec file opened successfully");

    char *temp_str = calloc(20, sizeof(char));
    dfa *d = create_dfa();

    fscanf(dfa_spec_fptr, "%s", temp_str);

    assert(strcmp(temp_str, "STATES") == 0, "expected keyword STATES found in dfa spec file");

    int num_states = 0;

    fscanf(dfa_spec_fptr, "%d", &num_states);

    assert(num_states > 0, "dfa spec file has > 0 states");

    while (num_states > 0)
    {
        int id, final_int;
        bool final;
        fscanf(dfa_spec_fptr, "%d %d", &id, &final_int);
        assert(final_int == 0 || final_int == 1, "final column valid in dfa spec file");
        if (final_int == 0)
        {
            final = false;
        }
        else if (final_int == 1)
        {
            final = true;
        }
        add_state(d, id, final);
        num_states--;
    }

    fscanf(dfa_spec_fptr, "%s", temp_str);

    assert(strcmp(temp_str, "TRANSITIONS") == 0, "expected keyword TRANSITIONS found in dfa spec file");

    char *transition_charset = calloc(30, sizeof(char));

    while (true)
    {
        int from_state, to_state;
        if (fscanf(dfa_spec_fptr, "%d %d %s", &from_state, &to_state, transition_charset) == EOF)
        {
            break;
        }
        else
        {
            if (strcmp(transition_charset, "other") == 0)
            {
                add_default_transition(d, from_state, to_state);
            }
            else
            {
                int transition_charset_idx = 0;
                while (transition_charset[transition_charset_idx] != '\0')
                {
                    if (transition_charset[transition_charset_idx + 1] == '|')
                    {
                        add_range_transition(d, from_state, to_state, transition_charset[transition_charset_idx], transition_charset[transition_charset_idx + 2]);
                        transition_charset_idx += 3;
                    }
                    else
                    {
                        add_transition(d, from_state, to_state, transition_charset[transition_charset_idx]);
                        transition_charset_idx++;
                    }
                }
            }
        }
    }

    free(transition_charset);

    return d;
}