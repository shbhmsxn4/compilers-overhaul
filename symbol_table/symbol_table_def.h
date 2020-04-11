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
#include "../lang_specs/entities.h"

typedef enum {
	integer,
	real,
	boolean,
	array
} id_type;

typedef struct ___VAR_ST_ENTRY___ {
	char lexeme[MAX_SYMBOL_LEN];
	id_type type;
	int width;
	int offset;
} var_id_entry;

typedef struct ___ARRAY_ST_ENTRY___ {
	char lexeme[MAX_SYMBOL_LEN];
	id_type type;
	int range_start;
	int range_end;
	int width; // for arrays, width is the width of single elem
	int offset; // TODO: after dynamic arrays are discussed
	bool is_static;
} arr_id_entry ;


typedef struct ___SCOPE_NODE___ scope_node;

typedef struct ___FUNC_ST_ENTRY___ func_entry;

struct ___SCOPE_NODE___ {
	hash_map* var_id_st;
	hash_map* arr_st;

	scope_node* parent_scope;
	hash_map* child_scopes;
	func_entry* func;
	var_id_entry *loop_var_entry;
};

struct ___FUNC_ST_ENTRY___ {
	char name[MAX_SYMBOL_LEN];
	linked_list* input_param_list;
	linked_list* output_param_list;
	scope_node* local_scope;
	bool is_declared;
	bool is_defined;
	bool is_called;
	int offset;
	int width;
};

typedef union ___ID_ST_ENTRY___ {
	var_id_entry *var_entry;
	arr_id_entry *arr_entry;
} id_entry; 

typedef struct ___PARAM_NODE___ {
	bool is_array;
	id_entry param;
	bool is_assigned; // for output params
} param_node;

typedef struct ___COMMON_ID_ENTRY___ {
	bool is_array;
	id_entry entry;
} common_id_entry;

typedef enum {
	for_decl,
	for_use,
	for_assign
} reason_flag;

#endif
