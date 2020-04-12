#include "../lang_specs/entities.h"
#include "./symbol_table_def.h"
#include "../ast/generate_ast.h"
#include "../data_structs/tree.h"

#define DEFAULT_ST_SIZE 71
#define DEFAULT_SCOPE_SIZE 41

id_type terminal_to_type (terminal t) {
	switch (t) {
		case INTEGER : return integer;
		case REAL : return real;
		case BOOLEAN : return boolean;
		case ARRAY : return array;
		case NUM : return integer;
		case RNUM: return real;
		case TRUE : return boolean;
		case FALSE : return boolean;
		default : return -1;
	}
}

id_type get_type_from_node (tree_node *node) {
	return terminal_to_type(
			((ast_node *) get_data(
				node))->label.gms.t);
}

int get_width_from_type (id_type t) {
	switch (t) {
		case integer : return 4;
		case real : return 8;
		case boolean : return 1;
		default : return -1;
	}
}

int* get_range_from_node (tree_node *node) {
	int *indices = (int *) malloc(2*sizeof(int));
	ast_leaf *index1 = (ast_leaf *) get_data(get_child(node, 0));
	ast_leaf *index2 = (ast_leaf *) get_data(get_child(node, 1));

	// if dynamic then range value is -1
	indices[0] = (index1->label.gms.t == ID) ? -1 : index1->ltk->nv.int_val;
	indices[1] = (index2->label.gms.t == ID) ? -1 : index2->ltk->nv.int_val;
	return indices;
}

scope_node *create_new_scope (scope_node *parent, func_entry *func) {
	scope_node *new_scope = (scope_node *) malloc(sizeof(scope_node));
	new_scope->var_id_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->arr_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->parent_scope = parent;
	new_scope->child_scopes = create_hash_map(DEFAULT_SCOPE_SIZE);
	new_scope->func = func;
	new_scope->loop_var_entry = NULL;

	return new_scope;
}

var_id_entry *create_var_entry (char *lexeme, id_type type, int width, int offset) {
	var_id_entry *entry = (var_id_entry *) malloc(sizeof(var_id_entry));
	strcpy(entry->lexeme, lexeme);
	entry->type = type;
	entry->width = (width != -1) ? width : get_width_from_type(type);
	entry->offset = offset;
	return entry;
}

arr_id_entry *create_arr_entry (char *lexeme, id_type type, int rstart, int rend, int width, int offset) {
	arr_id_entry *entry = (arr_id_entry *) malloc(sizeof(arr_id_entry));
	strcpy(entry->lexeme, lexeme);
	entry->type = type;
	entry->range_start = rstart;
	entry->range_end = rend;
	entry->width = (width != -1) ? width : get_width_from_type(type);
	entry->offset = offset;

	entry->is_static = (rstart != -1) && (rend != -1);
	return entry;
}

func_entry *create_func_entry (char *name, bool is_declared, bool is_defined, bool is_called, int offset, int width) {
	func_entry *entry = (func_entry *) malloc(sizeof(func_entry));
	strcpy(entry->name, name);
	entry->input_param_list = create_linked_list();
	entry->output_param_list = create_linked_list();
	entry->local_scope = create_new_scope(NULL, entry);
	entry->is_declared = is_declared;
	entry->is_defined = is_defined;
	entry->is_called = is_called;
	entry->offset = offset;
	entry->width = width;
	return entry;
}

common_id_entry *find_id (char *lexeme, scope_node *curr_scope, bool is_recursive) {
	common_id_entry *centry = NULL;

	var_id_entry *ventry = (var_id_entry *) fetch_from_hash_map(curr_scope->var_id_st, lexeme);
	if (ventry != NULL) {
		centry = (common_id_entry *) malloc(sizeof(common_id_entry));
		centry->is_array = false;
		centry->entry.var_entry = ventry;
		return centry;
	}

	arr_id_entry *aentry = (arr_id_entry *) fetch_from_hash_map(curr_scope->arr_st, lexeme);
	if (aentry != NULL) {
		centry = (common_id_entry *) malloc(sizeof(common_id_entry));
		centry->is_array = true;
		centry->entry.arr_entry = aentry;
		return centry;
	}

	if (is_recursive) {
		if (curr_scope->parent_scope != NULL)
			return find_id(lexeme, curr_scope->parent_scope, true);
	}
	return centry;
}

common_id_entry *find_id_rec (char *lexeme, scope_node *curr_scope) {
	return find_id (lexeme, curr_scope, true);
}

common_id_entry *find_id_in_scope (char *lexeme, scope_node *curr_scope) {
	return find_id (lexeme, curr_scope, false);
}

param_node *find_id_in_paramsll (char *lexeme, linked_list *pll) {
	for (int i = 0; i < pll->num_nodes; i++) {
		param_node *plnode = (param_node *) ll_get(pll, i);
		if (plnode->is_array) {
			arr_id_entry *aentry = plnode->param.arr_entry;
			if (strcmp(lexeme, aentry->lexeme) == 0) {
				return plnode;
			}
		}
		else {
			var_id_entry *ventry = plnode->param.var_entry;
			if (strcmp(lexeme, ventry->lexeme) == 0) {
				return plnode;
			}
		}
	}
	return NULL;
}

param_node *find_id_in_inputparams (char *lexeme, func_entry *entry) {
	linked_list *pll = entry->input_param_list;
	return find_id_in_paramsll(lexeme, pll);
}

param_node *find_id_in_outputparams (char *lexeme, func_entry *entry) {
	linked_list *pll = entry->output_param_list;
	return find_id_in_paramsll(lexeme, pll);
}

common_id_entry *param_to_st_entry (param_node *p) {
	if (p == NULL) return NULL;
	common_id_entry *centry = (common_id_entry *) malloc(sizeof(common_id_entry));
	centry->is_array = p->is_array;
	centry->entry = p->param;
	return centry;
}

common_id_entry *find_id_for_decl (char *lexeme, scope_node *curr_scope) {
	if (curr_scope->loop_var_entry != NULL && strcmp(lexeme, curr_scope->loop_var_entry->lexeme) == 0) {
		// TODO: loop var not allowed to use err
	}

	common_id_entry *sentry = find_id_in_scope(lexeme, curr_scope);
	if (sentry != NULL || curr_scope->parent_scope != NULL) return sentry;

	param_node *op = find_id_in_outputparams(lexeme, curr_scope->func);
	return param_to_st_entry(op);
}

common_id_entry *find_id_for_use (char *lexeme, scope_node *curr_scope) {
	common_id_entry *rentry = find_id_rec(lexeme, curr_scope);
	if (rentry != NULL) return rentry;

	param_node *ip = find_id_in_inputparams(lexeme, curr_scope->func);
	if (ip != NULL) return param_to_st_entry(ip);

	param_node *op = find_id_in_outputparams(lexeme, curr_scope->func);
	if (op != NULL && op->is_assigned) {
		return param_to_st_entry(op);
	}
	return NULL;
}

common_id_entry *find_id_for_assign (char *lexeme, scope_node *curr_scope) {
	if (curr_scope->loop_var_entry != NULL && strcmp(lexeme, curr_scope->loop_var_entry->lexeme) == 0) {
		printf("loop var should not be redefined in body\n");
		// TODO: loop var not allowed to use err
	}

	common_id_entry *rentry = find_id_rec(lexeme, curr_scope);
	if (rentry != NULL) return rentry;

	param_node *op = find_id_in_outputparams(lexeme, curr_scope->func);
	if (op != NULL) {
		op->is_assigned = true;
		return param_to_st_entry(op);
	}

	param_node *ip = find_id_in_inputparams(lexeme, curr_scope->func);
	return param_to_st_entry(ip);
}

common_id_entry *find_id_for (char *lexeme, scope_node *curr_scope, reason_flag need_for) {
	common_id_entry *ret;
	switch (need_for) {
		case for_decl : ret = find_id_for_decl(lexeme, curr_scope);
						break;
		case for_use : ret = find_id_for_use(lexeme, curr_scope);
					   break;
		case for_assign : ret = find_id_for_assign(lexeme, curr_scope);
						  break;
		default : assert(false, "invalid need_for reason flag\n");
				  ret = NULL;
	}
	return ret;
}

bool match_array_type (arr_id_entry *arr1, arr_id_entry *arr2) {
	return (arr1->type == arr2->type &&
		arr1->range_start == arr2->range_start &&
		arr1->range_end == arr2->range_end);
}

// returns
// -1 => dynamic bound check required
// 1 => check done 
int bound_type_check (arr_id_entry *entry, ast_leaf *index_data, scope_node *curr_scope) {
	if (index_data->label.gms.t == NUM) {
		if (entry->is_static) {
			// compile time bound check
			
			int ind = index_data->ltk->nv.int_val;
			if (ind < entry->range_start || ind > entry->range_end) {
				printf("index out of range - static arr & index\n");
				// TODO: out of bounds err
			}
			return 1;
		}
		else {
			// TODO: run time bound check - arr static but index dynamic
			printf("run time bound check - arr dynamic, index static\n");
		}
	}
	else {
		char *ind_var_name = index_data->ltk->lexeme;
		common_id_entry *ind_entry = find_id_for_use(ind_var_name, curr_scope);

		if (ind_entry == NULL) {
			printf("undeclared var : %s\n", ind_var_name);
			// TODO: 
		}
		else if (ind_entry->is_array || ind_entry->entry.var_entry->type != integer) {
			printf("index var should be integer\n");
			// TODO: 
		}
		// TODO: run time bound check for dynamic arrs
	}
	return -1;
}

common_id_entry *type_check_var (ast_leaf *id_data, ast_leaf *index_node, scope_node *curr_scope, reason_flag need_for) {
	char *var_name = id_data->ltk->lexeme;
	common_id_entry *entry = find_id_for(var_name, curr_scope, need_for);
	if (entry == NULL) {
		printf("undeclared var : %s\n", var_name);
		// TODO: undeclared id
	}
	else if (index_node != NULL) {
		if (!entry->is_array) {
			printf("var %s should be arr\n", var_name);
			// TODO: type err - should be array
		}
		else {
			arr_id_entry *aentry = entry->entry.arr_entry;
			bound_type_check(aentry, index_node, curr_scope);
			
			// TODO: generate asm code to compute expr
			// and assign it to array element of given index
		}
	}
	return entry;
}

// indexed => gives array elems type
id_type type_from_entry (common_id_entry *entry, bool indexed) {
	if (entry == NULL) return -1;
	if (entry->is_array) {
		if (indexed) return entry->entry.arr_entry->type;
		else return array;
	}
	else {
		return entry->entry.var_entry->type;
	}
}

bool is_same_type (common_id_entry *a, common_id_entry *b) {
	if (a->is_array != b->is_array) return false;
	else if (a->is_array) {
		arr_id_entry *aentry = a->entry.arr_entry;
		arr_id_entry *bentry = b->entry.arr_entry;
		return (aentry->type == bentry->type &&
				aentry->range_start == bentry->range_start &&
				aentry->range_end == bentry->range_end);
	}
	else {
		var_id_entry *aentry = a->entry.var_entry;
		var_id_entry *bentry = b->entry.var_entry;
		return (aentry->type == bentry->type);
	}
}

void arr_assign_offset (arr_id_entry *entry, func_entry *func) {
	int func_width = func->width;
	entry->offset = func_width;
	if (entry->is_static) {
		func->width = func_width + ((entry->range_end - entry->range_start + 1) * entry->width);
	}
	else {
		func->width = func_width + ASM_ADDR_SIZE;
	}
	printf("var width off : %s %d %d\n", entry->lexeme, entry->width, entry->offset);
}

void var_assign_offset (var_id_entry *entry, func_entry *func) {
	int func_width = func->width;
	entry->offset = func_width;
	func->width = func->width + entry->width;
	printf("var width off : %s %d %d\n", entry->lexeme, entry->width, entry->offset);
}

hash_map *create_symbol_table () {
	hash_map *main_st = create_hash_map(DEFAULT_ST_SIZE);

	// default entry for driver module
	func_entry *st_entry = create_func_entry("program", true, false, false, -1, 0);
	add_to_hash_map(main_st, st_entry->name, st_entry);

	return main_st;
}
