#include "variable_generator.h"

var_gen *create_var_gen()
{
    var_gen *vg = calloc(1, sizeof(var_gen));
    vg->num = 0;
    return vg;
}

void get_var(var_gen *vg, char *temp)
{
    temp[0] = 'L';
    itoa(lg->num, temp + 1, 10);
    (lg->num)++;
}

void destroy_label_gen(label_gen *lg)
{
    free(lg);
}