#include <stdbool.h>
#include "../lexer/lexer.h"
#include "../data_structs/tree.h"
#include "../data_structs/stack.h"
#include "../data_structs/grammar.h"
#include "../data_structs/parse_table.h"

#ifndef PARSER_H
#define PARSER_H

tree *parse(lexer *l, grammar *gm, parse_table *pt);

/*

Functions to be implemented:

parse

apply rule

*/

#endif