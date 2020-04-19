#include <stdlib.h>

#ifndef LABEL_GENERATOR_H
#define LABEL_GENERATOR_H

struct ____LABEL_GEN____ {
    int num;
};

typedef struct ____LABEL_GEN____ label_gen;

label_gen *create_label_gen();

void get_label(label_gen *lg, char *temp);

void destroy_label_gen(label_gen *lg);

#endif