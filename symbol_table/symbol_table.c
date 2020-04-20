/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "../lang_specs/entities.h"
#include "./symbol_table_def.h"
#include "./symbol_table.h"
#include "../ast/generate_ast.h"
#include "../data_structs/tree.h"
#include "../driver.h"

#define DEFAULT_ST_SIZE 71
#define DEFAULT_SCOPE_SIZE 41

// global `display_err_flag` used
void display_err (char *err_type, int line, char *err_msg) {
	compile_err = true;
	if (display_err_flag)
		printf("%s error at line %d : %s\n\n", err_type, line, err_msg);
}

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

char *type_to_str (id_type t) {
	switch (t) {
		case integer : return "integer";
		case real : return "real";
		case boolean : return "boolean";
		case array : return "array";
		default : return "unknown type";
	}
}

id_type get_type_from_node (tree_node *node) {
	return terminal_to_type(
			((ast_node *) get_data(
				node))->label.gms.t);
}

int get_width_from_type (id_type t) {
	switch (t) {
		case integer : return 2;
		case real : return 4;
		case boolean : return 1;
		default : return -1;
	}
}

int* get_static_range (tree_node *node) {
	int *indices = (int *) malloc(2*sizeof(int));
	ast_leaf *index1 = (ast_leaf *) get_data(get_child(node, 0));
	ast_leaf *index2 = (ast_leaf *) get_data(get_child(node, 1));

	// NOTE: no need to type check bcuz static range always has NUM
	// if dynamic then range value is -1
	indices[0] = (index1->label.gms.t == ID) ? -1 : index1->ltk->nv.int_val;
	indices[1] = (index2->label.gms.t == ID) ? -1 : index2->ltk->nv.int_val;
	return indices;
}

char** get_dynamic_range (tree_node *node, scope_node *curr_scope) {
	char **indices = (char **) malloc(2*sizeof(char*));
	indices[0] = NULL;
	indices[1] = NULL;
	ast_leaf *index1 = (ast_leaf *) get_data(get_child(node, 0));
	ast_leaf *index2 = (ast_leaf *) get_data(get_child(node, 1));

	// if static then range value is NULL 
	if (index1->label.gms.t == ID) {
		common_id_entry *var_entry = NULL;
		if (curr_scope != NULL)
			var_entry = type_check_var(index1, NULL, curr_scope, for_use);
		indices[0] = index1->ltk->lexeme;
		if (var_entry != NULL) {
			if (var_entry->is_array) {
				char err_msg[100];
				sprintf(err_msg, "range var '%s' should be of type integer, found array instead", index1->ltk->lexeme);
				display_err("Type", index1->ltk->line_num, err_msg);
			}
			else {
				var_id_entry *entry = var_entry->entry.var_entry;
				if (entry->type != integer) {
					char err_msg[100];
					sprintf(err_msg, "range var '%s' should be of type integer, found %s instead", index1->ltk->lexeme, type_to_str(entry->type));
					display_err("Type", index1->ltk->line_num, err_msg);
				}
			}
		}
	}

	if (index2->label.gms.t == ID) {
		common_id_entry *var_entry = NULL;
		if (curr_scope != NULL)
			var_entry = type_check_var(index2, NULL, curr_scope, for_use);
		indices[1] = index2->ltk->lexeme;
		if (var_entry != NULL) {
			if (var_entry->is_array) {
				char err_msg[100];
				sprintf(err_msg, "range var '%s' should be of type integer, found array instead", index2->ltk->lexeme);
				display_err("Type", index2->ltk->line_num, err_msg);
			}
			else {
				var_id_entry *entry = var_entry->entry.var_entry;
				if (entry->type != integer) {
					char err_msg[100];
					sprintf(err_msg, "range var '%s' should be of type integer, found %s instead", index2->ltk->lexeme, type_to_str(entry->type));
					display_err("Type", index2->ltk->line_num, err_msg);
				}
			}
		}
	}
			
	return indices;
}

scope_node *create_new_scope (scope_node *parent, func_entry *func, int sline, int eline) {
	scope_node *new_scope = (scope_node *) malloc(sizeof(scope_node));
	new_scope->var_id_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->arr_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->parent_scope = parent;
	new_scope->child_scopes = create_hash_map(DEFAULT_SCOPE_SIZE);
	new_scope->func = func;
	new_scope->loop_var_entry = NULL;
	new_scope->start_line = sline;
	new_scope->end_line = eline;

	new_scope->while_vars = create_linked_list(); 
	if (parent != NULL) {
		for (int i = 0; i < parent->while_vars->num_nodes; i++)
			ll_append(new_scope->while_vars, ll_get(parent->while_vars, i));
	}

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

arr_id_entry *create_arr_entry (char *lexeme, id_type type, int *rindices, char **rlexemes, int width, int offset) {
	arr_id_entry *entry = (arr_id_entry *) malloc(sizeof(arr_id_entry));
	strcpy(entry->lexeme, lexeme);
	entry->type = type;
	entry->range_start = rindices[0];
	entry->range_end = rindices[1];
	entry->rstart_lexeme = (rlexemes == NULL) ? NULL : rlexemes[0];
	entry->rend_lexeme = (rlexemes == NULL) ? NULL : rlexemes[1];
	entry->width = (width != -1) ? width : get_width_from_type(type);
	entry->offset = offset;

	entry->is_static = (rindices[0] != -1) && (rindices[1] != -1);
	return entry;
}

func_entry *create_func_entry (char *name, bool is_declared, bool is_defined, bool is_called, int offset, int width) {
	func_entry *entry = (func_entry *) malloc(sizeof(func_entry));
	strcpy(entry->name, name);
	entry->input_param_list = create_linked_list();
	entry->output_param_list = create_linked_list();
	entry->local_scope = create_new_scope(NULL, entry, 0, 0); // start, end line nos
															// assigned in moduleDef
	entry->is_declared = is_declared;
	entry->is_defined = is_defined;
	entry->is_called = is_called;
	entry->is_defined2 = false;
	entry->offset = offset;
	entry->width = width;
	return entry;
}

common_id_entry *find_id (char *lexeme, scope_node *curr_scope, bool is_recursive) {
	if (curr_scope == NULL) return NULL;
	common_id_entry *centry = NULL;

	var_id_entry *ventry = (var_id_entry *) fetch_from_hash_map(curr_scope->var_id_st, lexeme);
	if (ventry != NULL) {
		centry = (common_id_entry *) malloc(sizeof(common_id_entry));
		centry->is_array = false;
		centry->entry.var_entry = ventry;
		centry->is_param = false;
		return centry;
	}

	arr_id_entry *aentry = (arr_id_entry *) fetch_from_hash_map(curr_scope->arr_st, lexeme);
	if (aentry != NULL) {
		centry = (common_id_entry *) malloc(sizeof(common_id_entry));
		centry->is_array = true;
		centry->entry.arr_entry = aentry;
		centry->is_param = false;
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
	centry->is_param = true;
	return centry;
}

common_id_entry *find_id_for_decl (char *lexeme, scope_node *curr_scope, int line_num) {
	if (curr_scope->loop_var_entry != NULL && strcmp(lexeme, curr_scope->loop_var_entry->lexeme) == 0) {
		char err_msg[100];
		sprintf(err_msg, "Loop variable '%s' cannot be re-declared", lexeme);
		display_err("Semantic", line_num, err_msg);
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

common_id_entry *find_id_for_assign (char *lexeme, scope_node *curr_scope, int line_num) {
	if (curr_scope->loop_var_entry != NULL && strcmp(lexeme, curr_scope->loop_var_entry->lexeme) == 0) {
		char err_msg[100];
		sprintf(err_msg, "Loop variable '%s' cannot be assigned a value", lexeme);
		display_err("Semantic", line_num, err_msg);
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

common_id_entry *find_id_for (char *lexeme, scope_node *curr_scope, reason_flag need_for, int line_num) {
	common_id_entry *ret;
	switch (need_for) {
		case for_decl : ret = find_id_for_decl(lexeme, curr_scope, line_num);
						break;
		case for_use : ret = find_id_for_use(lexeme, curr_scope);
					   break;
		case for_assign : ret = find_id_for_assign(lexeme, curr_scope, line_num);
						  if (ret != NULL) {
							  for (int i = 0; i < curr_scope->while_vars->num_nodes; i++) {
								  param_node *n = ll_get(curr_scope->while_vars, i);
								  if (ret->is_array == n->is_array) {
									  if (n->is_array) {
										  if (n->param.arr_entry == ret->entry.arr_entry)
											  n->is_assigned = true;
									  }
									  else {
										  if (n->param.var_entry == ret->entry.var_entry)
											  n->is_assigned = true;
									  }
								  }
							  }
						  }
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
				char err_msg[100];
				sprintf(err_msg, "index '%d' is out of bounds for array '%s'", ind, entry->lexeme);
				display_err("Semantic", index_data->ltk->line_num, err_msg);
			}
			return 1;
		}
		else {
			// TODO: run time bound check - arr static but index dynamic
			/*printf("run time bound check - arr dynamic, index static\n");*/
		}
	}
	else {
		char *ind_var_name = index_data->ltk->lexeme;
		common_id_entry *ind_entry = type_check_var(index_data, NULL, curr_scope, for_use);

		if (ind_entry != NULL) {
			char err_msg[100];
			if (ind_entry->is_array) {
				sprintf(err_msg, "index '%s' should be integer, found array", ind_var_name);
				display_err("Semantic", index_data->ltk->line_num, err_msg);
			}
			else if (ind_entry->entry.var_entry->type != integer) {
				sprintf(err_msg, "index '%s' should be integer, found %s", ind_var_name, type_to_str(ind_entry->entry.var_entry->type));
				display_err("Semantic", index_data->ltk->line_num, err_msg);
			}
		}
		// TODO: run time bound check for dynamic arrs
	}
	return -1;
}

common_id_entry *type_check_var (ast_leaf *id_data, ast_leaf *index_node, scope_node *curr_scope, reason_flag need_for) {
	char *var_name = id_data->ltk->lexeme;
	common_id_entry *entry = find_id_for(var_name, curr_scope, need_for, id_data->ltk->line_num);
	if (entry == NULL) {
		char err_msg[100];
		sprintf(err_msg, "'%s' variable undeclared", var_name);
		display_err("Semantic", id_data->ltk->line_num, err_msg);
	}
	else if (index_node != NULL) {
		if (!entry->is_array) {
			char err_msg[100];
			sprintf(err_msg, "'%s' variable should be an array to be indexed, found %s", var_name, type_to_str(entry->entry.var_entry->type));
			display_err("Type", id_data->ltk->line_num, err_msg);
		}
		else {
			arr_id_entry *aentry = entry->entry.arr_entry;
			bound_type_check(aentry, index_node, curr_scope);
			
			// TODO: generate asm code to compute expr
			// and assign it to array element of given index
		}
	}

	if (is_while_expr && need_for == for_use) {
		param_node *n = (param_node *) malloc(sizeof(param_node));
		n->is_array = entry->is_array;
		n->param = entry->entry;
		n->is_assigned = false;
		ll_append(while_scope->while_vars, n);
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
		
		if (aentry->is_static && bentry->is_static) {
			return (aentry->type == bentry->type &&
				aentry->range_start == bentry->range_start &&
				aentry->range_end == bentry->range_end);
		}
		else {
			return (aentry->type == bentry->type);
		}
	}
	else {
		var_id_entry *aentry = a->entry.var_entry;
		var_id_entry *bentry = b->entry.var_entry;
		return (aentry->type == bentry->type);
	}
}

void arr_assign_offset (arr_id_entry *entry, func_entry *func, bool is_param) {
	int func_width = func->width;
	entry->offset = func_width;
	if (is_param) {
		func->width = func_width + (2 * get_width_from_type(integer)) + 1;
	}
	else {
		if (entry->is_static) {
			func->width = func_width + ((entry->range_end - entry->range_start + 1) * entry->width) + 1;
		}
		else {
			/*func->width = func_width + ASM_ADDR_SIZE;*/
			func->width = func_width + 1;
		}
	}
	/*printf("var width off : %s %d %d\n", entry->lexeme, entry->width, entry->offset);*/
}

void var_assign_offset (var_id_entry *entry, func_entry *func) {
	int func_width = func->width;
	entry->offset = func_width;
	func->width = func->width + entry->width;
	/*printf("var width off : %s %d %d\n", entry->lexeme, entry->width, entry->offset);*/
}

hash_map *create_symbol_table () {
	hash_map *main_st = create_hash_map(DEFAULT_ST_SIZE);

	// default entry for driver module
	func_entry *st_entry = create_func_entry("driver", true, false, false, -1, 0);
	add_to_hash_map(main_st, st_entry->name, st_entry);

	return main_st;
}

void print_var_entry (var_id_entry *entry, scope_node *curr_scope, int level) {
	char scope_lines[25];
	sprintf(scope_lines, "%d-%d", curr_scope->start_line, curr_scope->end_line);
	printf("%-15s| %-15s| %-15s| %-10d| %-10s| %-15s| %-15s| %-10s| %-10d| %-10d\n",
			entry->lexeme, curr_scope->func->name, scope_lines, entry->width, "no", "---", "---",
			type_to_str(entry->type), entry->offset, level);

}

void print_arr_entry (arr_id_entry *entry, scope_node *curr_scope, int level, bool is_param) {
	char scope_lines[25];
	sprintf(scope_lines, "%d-%d", curr_scope->start_line, curr_scope->end_line);

	int width;
	if (is_param) {
		width = (2 * get_width_from_type(integer)) + 1;
	}
	else {
		if (entry->is_static) {
			width = ((entry->range_end - entry->range_start + 1) * entry->width) + 1;
		}
		else {
			width = 1;
		}
	}

	char static_or_dynamic[10];
	sprintf(static_or_dynamic, (entry->is_static) ? "static" : "dynamic");

	if (entry == NULL) return;
	char rstart[20];
	if (entry->range_start == -1) {
		char *rlex = entry->rstart_lexeme;
		if (rlex != NULL) {
			sprintf(rstart, "%s", rlex);
		}
		else strcpy(rstart, "-");
	}
	else
		sprintf(rstart, "%d", entry->range_start);

	char rend[20];
	if (entry->range_end == -1) {
		char *rlex = entry->rend_lexeme;
		if (rlex != NULL) {
			sprintf(rend, "%s", rlex);
		}
		else strcpy(rend, "-");
	}
	else
		sprintf(rend, "%d", entry->range_end);

	char range[40];
	sprintf(range, "[%s, %s]", rstart, rend);

	if (array_only_flag) {
		printf("%-15s| %-15s| %-15s| %-15s| %-15s| %-15s\n",
			curr_scope->func->name, scope_lines, entry->lexeme, static_or_dynamic,
			range, type_to_str(entry->type));
	}
	else {
		printf("%-15s| %-15s| %-15s| %-10d| %-10s| %-15s| %-15s| %-10s| %-10d| %-10d\n",
			entry->lexeme, curr_scope->func->name, scope_lines, width, "yes", static_or_dynamic,
			range, type_to_str(entry->type), entry->offset, level);

	}
}

void print_scope (scope_node *curr_scope, int level) {
	if (!array_only_flag) {
		hm_node *var_list = get_all_hm_nodes(curr_scope->var_id_st);
		while (var_list != NULL) {
			print_var_entry(var_list->data, curr_scope, level);
			var_list = var_list->next;
		}
	}

	hm_node *arr_list = get_all_hm_nodes(curr_scope->arr_st);
	while (arr_list != NULL) {
		print_arr_entry(arr_list->data, curr_scope, level, false);
		arr_list = arr_list->next;
	}

	hm_node *child_scope_list = get_all_hm_nodes(curr_scope->child_scopes);
	while (child_scope_list != NULL) {
		print_scope(child_scope_list->data, level+1);
		child_scope_list = child_scope_list->next;
	}
}

void print_param_list (linked_list *plist, scope_node *curr_scope) {
	ll_node *phead = plist->head;
	while (phead) {
		param_node *pnode = (param_node *) phead->data;
		if (pnode->is_array) {
			print_arr_entry(pnode->param.arr_entry, curr_scope, 0, true);
		}
		else if (!array_only_flag) {
			print_var_entry(pnode->param.var_entry, curr_scope, 0);
		}

		phead = phead->next;
	}
}

void print_symbol_table (hash_map *st) {
	printf("\n********* SYMBOL TABLE *********\n\n");

	if (array_only_flag) {
		printf("%-15s| %-15s| %-15s| %-15s| %-15s| %-15s\n",
			"Scope", "scope lines", "Var name", "static/dynamic", "range", "type");
		printf("%-15s| %-15s| %-15s| %-15s| %-15s| %-15s\n",
			"========", "========", "========", "========", "========", "========");
	}
	else {
		printf("%-15s| %-15s| %-15s| %-10s| %-10s| %-15s| %-15s| %-10s| %-10s| %-10s\n",
			"Var name", "Scope", "scope lines", "width", "is array", "static/dynamic", "range",
			"type", "offset", "nesting level");
		printf("%-15s| %-15s| %-15s| %-10s| %-10s| %-15s| %-15s| %-10s| %-10s| %-10s\n",
			"========", "========", "========", "========", "========", "========", "========",
			"========", "========", "========");
	}


	hm_node *module_list = get_all_hm_nodes(st);
	while (module_list != NULL) {
		func_entry *module_entry = (func_entry *) module_list->data;

		// input params
		linked_list *ip_ll = module_entry->input_param_list;
		print_param_list(ip_ll, module_entry->local_scope);
		
		// output params
		linked_list *op_ll = module_entry->output_param_list;
		print_param_list(op_ll, module_entry->local_scope);

		print_scope(module_entry->local_scope, 1);
		module_list = module_list->next;
	}
}

void print_ar_size (hash_map *st) {
	printf("\n********* ACTIVATION RECORD SIZE *********\n\n");
	printf("%-15s| %-10s\n", "Module", "Size");
	printf("%-15s| %-10s\n", "========", "========");

	hm_node *module_list = get_all_hm_nodes(st);
	while (module_list != NULL) {
		func_entry *module_entry = (func_entry *) module_list->data;

		printf("%-15s| %-10d\n\n", module_entry->name, module_entry->width);

		module_list = module_list->next;
	}
}
