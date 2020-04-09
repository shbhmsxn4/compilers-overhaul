#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../data_structs/tree.h"
#include "./symbol_table_def.h"

void create_symbol_table (tree *astn);
id_type terminal_to_type (terminal t);

#endif
