#include "../data_structs/grammar.h"
#include "../data_structs/parse_table.h"
#include "../lang_specs/entities.h"

#ifndef GENERATE_PARSE_TABLE_H
#define GENERATE_PARSE_TABLE_H

parse_table * generate_parse_table(grammar *gm, gm_first * fi, gm_follow *fo);


#endif