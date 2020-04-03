#include "../lang_specs/entities.h"
#include "grammar.h"

#ifndef PARSE_TABLE_H
#define PARSE_TABLE_H

struct ____PARSE_TABLE____
{
    gm_rule ***matrix;
    int num_rows;
    int num_cols;
};

typedef struct ____PARSE_TABLE____ parse_table;

parse_table *create_parse_table(int num_rows, int num_cols);

void add_to_parse_table(parse_table *pt, gm_rule *rule, int r, int c);

gm_rule *get_from_parse_table(parse_table *pt, int r, int c);

void destroy_parse_table(parse_table *pt);

#endif