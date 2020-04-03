#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../data_structs/dfa.h"
#include "../utils/gen_utils.h"

#ifndef PARSE_DFA_H
#define PARSE_DFA_H

dfa *parse_dfa(char *dfa_spec_filepath);

#endif