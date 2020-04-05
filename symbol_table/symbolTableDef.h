/*
 Group- 35
 2017A7PS0082P		Laksh Singla
 2017A7PS0148P 		Kunal Mohta
 2017A7PS0191P 		Suyash Raj
 2017A7PS0302P 		Shubham Saxena
*/

#ifndef _SYMTABDEF_H_
#define _SYMTABDEF_H_

#include <stdbool.h>
#include "../data_structs/hash_map.h"
#include "../data_structs/linked_list.h"
#include "../data_structs/tree.h"
#include "../lang_specs/entities.h"
#include "../ast/generate_ast.h"

typedef enum {
	integer,
	real,
	boolean,
	array,
	function
} id_type;

typedef struct ___VAR_ST_ENTRY___ {
	id_type type;
	int width;
	int offset;
	char lexeme[MAX_SYMBOL_LEN];
} var_id_entry;

typedef struct ___ARRAY_ST_ENTRY___ {
	id_type t;
	int width; // TODO: after dynamic arrays are discussed
	int offset;
	int range_start;
	int range_end;
	char lexeme[MAX_SYMBOL_LEN];
} arr_id_entry ;

typedef union ___ID_ST_ENTRY___ {
	var_id_entry *var_entry;
	arr_id_entry *arr_entry;
} id_entry; 

typedef struct ___SCOPE_NODE___ scope_node;

struct ___SCOPE_NODE___ {
	hash_map* var_id_st;
	hash_map* arr_st;

	scope_node* parent_scope;
	linked_list* child_scopes;
};

typedef struct ___FUNC_ST_ENTRY___ {
	char name[MAX_SYMBOL_LEN];
	linked_list* input_param_list;
	linked_list* output_param_list;
	int offset;
	int width;
	scope_node* local_scope;
	bool only_declared;
	bool is_called;
} func_entry;

typedef struct ___PARAM_NODE___ {
	bool is_array;
	id_entry param;
} param_node;

#endif
