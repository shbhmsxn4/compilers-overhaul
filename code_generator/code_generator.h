#include <stdbool.h>
#include <stdlib.h>
#include "../data_structs/tree.h"
#include "../parser/parser.h"
#include "../utils/gen_utils.h"
#include "../lang_specs/entities.h"
#include "../symbol_table/symbol_table.h"
#include "../symbol_table/symbol_table_def.h"
#include "label_generator.h"

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#define MAX_OFFSET_DIGS 4
#define MAX_LABEL_LEN 5
#define MAX_INT_LEN 8

code *create_empty_code();

void append_code(code *c, char *str);

void prepend_code(code *c, char *str);

void stitch_code_append(tree_node *dst, tree_node *src);

void stitch_code_prepend(code *dst, code *src);

void generate_code(tree_node *n, hash_map *st, scope_node *curr_scope, label_gen *lg);

#endif