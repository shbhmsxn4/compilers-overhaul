#include <stdlib.h>

#ifndef VARIABLE_GENERATOR_H
#define VARIABLE_GENERATOR_H

struct ____VARIABLE_GEN____
{
    int num;
};

typedef struct ____VARIABLE_GEN____ var_gen;

var_gen *create_var_gen();

void get_var(var_gen *vg, char *temp);

void destroy_var_gen(var_gen *vg);

#endif