#include "label_generator.h"

label_gen *create_label_gen()
{
    label_gen *lg = calloc(1, sizeof(label_gen));
    lg->num = 0;
    return lg;
}

void get_label(label_gen *lg, char *temp)
{
    temp[0] = 'L';
    itoa(lg->num, temp + 1, 10);
    (lg->num)++;
}

void destroy_label_gen(label_gen *lg)
{
    free(lg);
}