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
#include "hashTable.h"
#include "linkedlist.h"
#include "constants.h"

typedef enum {
	integer,
	real,
	boolean,
	array,
	function
} type;

typedef struct normal_id_st_entry {
	type t;
	int width;
	int offset;
	char lexeme[__MAX_SYMBOL_LEN__];
} VarIdEntry;

typedef struct array_st_entry {
	type t;
	int width; // TODO: after dynamic arrays are discussed
	int offset;
	int range_start;
	int range_end;
	char lexeme[__MAX_SYMBOL_LEN__];
} ArrEntry;

typedef union id_entry {
	VarIdEntry var_id_entry;
	ArrEntry arr_entry;
} IdEntry; 

typedef struct scope {
	HashTable* var_id_st;
	HashTable* arr_st;

	struct scope* parent_scope;
	LinkedList* child_scopes;
} ScopeNode;

typedef struct func_st_entry {
	char name[__MAX_SYMBOL_LEN__];
	LinkedList* input_param_list;
	LinkedList* output_param_list;
	int offset;
	int width;
	ScopeNode* local_scope;
} FuncEntry;

typedef struct param_node {
	bool is_array;
	IdEntry param;
} PNode;

#endif
