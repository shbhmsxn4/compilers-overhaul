/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdbool.h>
#include "../lexer/lexer.h"
#include "../data_structs/tree.h"
#include "../data_structs/stack.h"
#include "../data_structs/grammar.h"
#include "../data_structs/parse_table.h"
#include "../utils/print.h"

#ifndef PARSER_H
#define PARSER_H

tree *parse(lexer *l, grammar *gm, parse_table *pt, gm_follow *fo);

/*

Functions to be implemented:

parse

apply rule

*/

#endif
