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

func_entry *create_func_entry (char *name, bool only_declared, bool is_called, int offset, int width) {
	func_entry *entry = (func_entry *) malloc(sizeof(func_entry));
	strcpy(entry->name, name);
	entry->input_param_list = create_linked_list();
	entry->output_param_list = create_linked_list();
	entry->local_scope = create_new_scope(NULL, entry);
	entry->only_declared = only_declared;
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
	/*common_id_entry *centry = (common_id_entry *) malloc(sizeof(common_id_entry));*/
	for (int i = 0; i < pll->num_nodes; i++) {
		param_node *plnode = (param_node *) ll_get(pll, i);
		if (plnode->is_array) {
			arr_id_entry *aentry = plnode->param.arr_entry;
			if (strcmp(lexeme, aentry->lexeme) == 0) {
				/*
				 *centry->is_array = true;
				 *centry->entry = plnode->param;
				 *break;
				 */
				return plnode;
			}
		}
		else {
			var_id_entry *ventry = plnode->param.var_entry;
			if (strcmp(lexeme, ventry->lexeme) == 0) {
				/*
				 *centry->is_array = false;
				 *centry->entry = plnode->param;
				 *break;
				 */
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

bool match_array_type (arr_id_entry *arr1, arr_id_entry *arr2) {
	return (arr1->type == arr2->type &&
		arr1->range_start == arr2->range_start &&
		arr1->range_end == arr2->range_end);
}

// returns
// -1 => dynamic bound check required
// 1 => check done 
int bound_type_check (arr_id_entry *entry, tree_node *index_nt) {
	ast_leaf *index_data = (ast_leaf *) get_data(index_nt);
	if (entry->is_static) {
		if (index_data->label.gms.t == NUM) {
			// compile time bound check
			
			int ind = index_data->ltk->nv.int_val;
			if (ind >= entry->range_start && ind <= entry->range_end) {
				// TODO: out of bounds err
			}
			return 1;
		}
		else {
			// TODO: run time bound check - arr static but index dynamic
			
			if (get_type_from_node(index_nt) != integer) {
				// TODO: err index should be of type integer
			}
			return -1;
		}
	}
	else {
		if (index_data->label.gms.t == NUM) {
			// TODO: run time bound check - arr dynamic but index static 

			return -1;
		}
		else {
			// TODO: run time bound check - arr dynamic but index dynamic

			return -1;
		}
	}
}

// FIRST AST PASS
void symbol_table_fill (hash_map *main_st, tree_node *astn, scope_node *curr_scope) {
	ast_node* ast_data = (ast_node *)get_data(astn);
	gm_unit data_label = ast_data->label;
	
	int num_children = get_num_children(astn);

	nonterminal ast_nt = data_label.gms.nt;

	char ast_nt_name[100];
	nonterminal_name(ast_nt, ast_nt_name);
	printf("here %s\n", ast_nt_name);
    if (ast_nt == program) {
		symbol_table_fill(main_st, get_child(astn, 0), curr_scope);
		symbol_table_fill(main_st, get_child(astn, 1), curr_scope);

		// driver module
		func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
		assert(st_entry != NULL, "default entry for driver module not found");
		st_entry->only_declared = false;
		symbol_table_fill(main_st, get_child(astn, 2), st_entry->local_scope);

		symbol_table_fill(main_st, get_child(astn, 3), curr_scope);
    }

    if (ast_nt == moduleDeclarations) {
		linked_list *module_decs = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < module_decs->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(module_decs, i), curr_scope);
    }

	if (ast_nt == moduleDeclaration) {
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "moduleDeclaration : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, true, false, -1, -1);
			add_to_hash_map(main_st, func_name, f_st_entry);
		}
		else {
			// TODO: module redeclaration error
		}
		//
		//
	}

	if (ast_nt == otherModules) {
		linked_list *other_mods = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < other_mods->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(other_mods, i), curr_scope);
	}

	/*
	 *if (ast_nt == driverModule) {
	 *    func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
	 *    assert(st_entry != NULL, "default entry for driver module not found");
	 *    st_entry->only_declared = false;
	 *    //
	 *    //
	 *}
	 */

	if (ast_nt == module) {
		// MODULE ID
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "module : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, false, false, -1, -1);
			add_to_hash_map(main_st, func_name, f_st_entry);
		}
		else if (f_st_entry->only_declared && f_st_entry->is_called) {
			f_st_entry->only_declared = false;
		}
		else if (f_st_entry->only_declared && !f_st_entry->is_called) {
			// TODO: redundant declaration, only definition required
		}
		else {
			// TODO: module redefinition error
		}

		// MODULE INPUT PARAM LIST
		tree_node* module_iplist = get_child(astn, 1);
		ast_node* module_iplist_data = (ast_node *) get_data(module_iplist);

		assert(!module_iplist_data->label.is_terminal && module_iplist_data->label.gms.nt == input_plist, "module : need input_plist here");

		linked_list *ip_ll = module_iplist_data->ll;
		linked_list *fip_ll = create_linked_list();
		for (int i = 0; i < ip_ll->num_nodes; i++) {
			param_node *fparam = (param_node *) malloc(sizeof(param_node));
			tree_node *param = (tree_node *) ll_get(ip_ll, i);
			ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
			tree_node *param_type = get_child(param, 1);
			id_type type_param = get_type_from_node(param_type);

			if (type_param == array) {
				fparam->is_array = true;
				
				tree_node *range_arrays = get_child(param_type, 0);
				int *range_indices = get_range_from_node(range_arrays);

				fparam->param.arr_entry = create_arr_entry(
						param_id->ltk->lexeme,
						get_type_from_node(get_child(param_type, 1)),
						range_indices[0],
						range_indices[1],
						-1, -1);
			}
			else {
				fparam->is_array = false;
				fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);
			}
			ll_append(fip_ll, fparam);
		}
		f_st_entry->input_param_list = fip_ll;

		// MODULE OUTPUT PARAM LIST
		tree_node* module_oplist = get_child(astn, 2);
		ast_node* module_oplist_data = (ast_node *) get_data(module_oplist);

		assert(!module_oplist_data->label.is_terminal && module_oplist_data->label.gms.nt == output_plist, "module : need output_plist here");

		linked_list *op_ll = module_oplist_data->ll;
		linked_list *fop_ll = create_linked_list();
		for (int i = 0; i < op_ll->num_nodes; i++) {
			param_node *fparam = (param_node *) malloc(sizeof(param_node));
			tree_node *param = (tree_node *) ll_get(op_ll, i);
			ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
			tree_node *param_type = get_child(param, 1);
			id_type type_param = get_type_from_node(param_type);

			fparam->is_array = false;
			fparam->is_assigned = false;
			fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);

			ll_append(fop_ll, fparam);
		}
		f_st_entry->output_param_list = fop_ll;

		// MODULEDEF
		// TODO: TEMPORARY????
		symbol_table_fill(main_st, get_child(astn, 3), f_st_entry->local_scope);

		// check if all output params have been assigned
		for (int i = 0; i < op_ll->num_nodes; i++) {
			param_node *pnode = (param_node *) ll_get(op_ll, i);
			if (!pnode->is_assigned) {
				// TODO: err unassigned output param
			}
		}
	}

	if (ast_nt == moduleDef) {
/*
 *        tree_node *statemens_node = get_child(astn, 0); // TODO: TEMP???
 *        linked_list *statements = ((ast_node *) get_data(statemens_node))->ll;
 *
 *        for (int i = 0; i < statements->num_nodes; i++) {
 *            symbol_table_fill(main_st, ll_get(statements, i), curr_scope);
 *        }
 */
		symbol_table_fill(main_st, get_child(astn, 0), curr_scope);
	}

	if (ast_nt == statements) {
		linked_list *statements = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < statements->num_nodes; i++) {
			symbol_table_fill(main_st, ll_get(statements, i), curr_scope);
		}
	}

	if (ast_nt == var_id_num) {
		int child_cnt = get_num_children(astn);
		tree_node *var_node = astn;
		ast_node *var_data = (ast_node *) get_data(astn);
		if (child_cnt == 1) {
			// VAR -> NUM, RNUM, TRUE, FALSE
			tree_node *const_var_node = get_child(var_node, 0);
			ast_leaf *const_var_data = (ast_leaf *) get_data(const_var_node);
			var_data->type = const_var_data->type;
			// TODO: generate ast code to display value of
			// this constant
		}
		else if (child_cnt == 2) {
			ast_leaf *id_data = (ast_leaf *) get_data(get_child(var_node, 0));
			tree_node *whichid_node = get_child(var_node, 1);
			
			char *var_name = id_data->ltk->lexeme;
			common_id_entry *entry = find_id_for_use(var_name, curr_scope);
			if (entry == NULL) {
				// TODO: undeclared var err
				printf("undeclared var : %s\n", var_name);
			}
			else if (get_data(whichid_node) == NULL) {
				// VAR -> ID
				if (entry->is_array) {
					var_data->type = array;
					// TODO: assembly code to display all
					// elements of the array
				}
				else {
					var_data->type = entry->entry.var_entry->type;
					// TODO: generate ast code to get value of var
					// and display it
				}
			}
			else {
				// VAR -> ID WHICHID
				ast_leaf *whichid_data = (ast_leaf *) get_data(whichid_node);
				if (!entry->is_array) {
					// TODO: type err
				}
				else {
					arr_id_entry *aentry = entry->entry.arr_entry;

					var_data->type = aentry->type;

					int check_ret = bound_type_check(aentry, whichid_node);
					if (check_ret == 1) {
						// TODO: out of bounds err
					}
					else if (check_ret == -1) {
						// TODO: runtime bound check
						// here or in bound_check() call???
					}
					
					// TODO: generate ast code to index arr
					// and display corresponding value
				}
			}
		}
	}

	/*ioStmt start*/
	if (ast_nt == input_stmt) {
		char *var_name = ((ast_leaf *) get_data(get_child(astn, 0)))->ltk->lexeme;
		common_id_entry *entry = find_id_for_use(var_name, curr_scope);
		if (entry == NULL) {
			// TODO: variable not declared
		}
		else {
			// TODO:
			// generate assembly code to take input
			// and store at corresponding offset
			// taken from ST
		}
	}

	if (ast_nt == output_stmt) {
		tree_node *var_node = get_child(astn, 0);
		
		if (((ast_leaf *) get_data(var_node))->is_leaf) {
			printf("output stmt printing const\n");
		}
		else {
			symbol_table_fill(main_st, var_node, curr_scope);
		}
		/*
		 *if (child_cnt == 1) {
		 *    // VAR -> NUM, RNUM, TRUE, FALSE
		 *    tree_node *const_var_node = get_child(var_node, 0);
		 *    // TODO: generate ast code to display value of
		 *    // this constant
		 *}
		 *else if (child_cnt == 2) {
		 *    ast_leaf *id_data = (ast_leaf *) get_data(get_child(var_node, 0));
		 *    tree_node *whichid_node = get_child(var_node, 1);
		 *    
		 *    char *var_name = id_data->ltk->lexeme;
		 *    common_id_entry *entry = find_id_for_use(var_name, curr_scope);
		 *    if (get_data(whichid_node) == NULL) {
		 *        // VAR -> ID
		 *        if (entry->is_array) {
		 *            // TODO: assembly code to display all
		 *            // elements of the array
		 *        }
		 *        else {
		 *            // TODO: generate ast code to get value of var
		 *            // and display it
		 *        }
		 *    }
		 *    else {
		 *        // VAR -> ID WHICHID
		 *        ast_leaf *whichid_data = (ast_leaf *) get_data(whichid_node);
		 *        if (!entry->is_array) {
		 *            // TODO: type err
		 *        }
		 *        else {
		 *            arr_id_entry *aentry = entry->entry.arr_entry;
		 *            int check_ret = bound_type_check(aentry, whichid_node);
		 *            if (check_ret == 1) {
		 *                // TODO: out of bounds err
		 *            }
		 *            else if (check_ret == -1) {
		 *                // TODO: runtime bound check
		 *                // here or in bound_check() call???
		 *            }
		 *            
		 *            // TODO: generate ast code to index arr
		 *            // and display corresponding value
		 *        }
		 *    }
		 *}
		 */
	}
	/*ioStmt end*/

	if (ast_nt == assignmentStmt) {
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0));
		tree_node *whichstmt_node = get_child(astn, 1);
		ast_node *whichstmt_data = (ast_node *) get_data(whichstmt_node);
		
		char *var_name = id_data->ltk->lexeme;
		common_id_entry *entry = find_id_for_assign(var_name, curr_scope);
		if (entry == NULL) {
			// TODO: undeclared id
		}
		else {
			if (whichstmt_data->label.gms.nt == lvalueIDStmt) {
				if (entry->is_array) {
					// TODO: type err - should not be array
				}
				else {
					tree_node *expr_node = get_child(whichstmt_node, 0);
					symbol_table_fill(main_st, expr_node, curr_scope);
					// TODO: generate assembly code to compute expr value
					// and assign it to the id here
				}
			}
			else if (whichstmt_data->label.gms.nt == lvalueARRStmt) {
				if (!entry->is_array) {
					// TODO: type err - should be array
				}
				else {
					tree_node *index_node = get_child(whichstmt_node, 0);
					tree_node *expr_node = get_child(whichstmt_node, 1);

					ast_leaf *index_data = (ast_leaf *) get_data(index_node);
					if (index_data->label.gms.t == NUM) {
						// TODO:
					}
					else if (index_data->label.gms.t == ID) {
						// TODO:
					}
					// TODO: generate asm code to compute expr
					// and assign it to array element of given index
				}
			}
		}
	}

	if (ast_nt == moduleReuseStmt) {
		tree_node *opt_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 1));

		char *func_name = id_data->ltk->lexeme;
		func_entry *fentry = fetch_from_hash_map(main_st, func_name);

		if (fentry == curr_scope->func) {
			// TODO: recursion not allowed
		}

		if (fentry == NULL) {
			// TODO: undeclared func err
			printf("ERR: undeclared module\n");
			return;
		}

		// OUTPUT PARAMS - return values
		if (get_data(opt_node) != NULL) {
			linked_list *ret_var_ll = ((ast_node *) get_data(opt_node))->ll;
			linked_list *op_ll = fentry->output_param_list;

			if (ret_var_ll->num_nodes != op_ll->num_nodes) {
				// TODO: params count mismatch err
			}
			else {
				int pcnt = op_ll->num_nodes;
				for (int i = 0; i < pcnt; i++) {
					tree_node *ret_var_lnode = (tree_node *) ll_get(ret_var_ll, i);
					char *ret_var_name = ((ast_leaf *) get_data(ret_var_lnode))->ltk->lexeme;
					common_id_entry *ret_var_entry = find_id_for_assign(ret_var_name, curr_scope);
					if (ret_var_entry == NULL) {
						// TODO: undeclared var
					}
					else {
						param_node *op_lnode = (param_node *) ll_get(op_ll, i);
						if (
								(ret_var_entry->is_array != op_lnode->is_array) ||
								(ret_var_entry->is_array && 
								 !match_array_type(ret_var_entry->entry.arr_entry, op_lnode->param.arr_entry)
								 ) ||
								(!ret_var_entry->is_array &&
								 ret_var_entry->entry.var_entry->type != op_lnode->param.var_entry->type
								 )
						   ) {
							// TODO: type mismatch between function call and assign
						}
					}
				}
			}
		}

		// INPUT PARAMS - args
		linked_list *arg_var_ll = ((ast_node *) get_data(get_child(astn, 2)))->ll;
		linked_list *ip_ll = fentry->input_param_list;

		if (arg_var_ll->num_nodes != ip_ll->num_nodes) {
			// TODO: params count mismatch err
		}
		else {
			int pcnt = ip_ll->num_nodes;
			for (int i = 0; i < pcnt; i++) {
				tree_node *arg_var_lnode = (tree_node *) ll_get(arg_var_ll, i);
				char *arg_var_name = ((ast_leaf *) get_data(arg_var_lnode))->ltk->lexeme;
				common_id_entry *arg_var_entry = find_id_for_use(arg_var_name, curr_scope);
				if (arg_var_entry == NULL) {
					// TODO: undeclared var
				}
				else {
					param_node *ip_lnode = (param_node *) ll_get(ip_ll, i);
					if (
							(arg_var_entry->is_array != ip_lnode->is_array) ||
							(arg_var_entry->is_array && 
							 !match_array_type(arg_var_entry->entry.arr_entry, ip_lnode->param.arr_entry)
							 ) ||
							(!arg_var_entry->is_array &&
							 arg_var_entry->entry.var_entry->type != ip_lnode->param.var_entry->type
							 )
					   ) {
						// TODO: type mismatch between function call and assign
					}
				}
			}
		}
	}
    
	if (ast_nt == expression) {

	}

	/*
     *if (ast_nt == unary_nt)
     *{
     *}
	 */

    if (ast_nt == new_NT)
    {
    }

    /*if (ast_nt == unary_op) {*/
    if (ast_nt == unary_nt) {
		ast_leaf *uop = (ast_leaf *) get_data(get_child(astn, 0));
		printf("unary operator found\n");
		// TODO: what to do with the unary operator here??
		symbol_table_fill(main_st, get_child(astn, 1), curr_scope);
	}

    if (ast_nt == arithmeticOrBooleanExpr)
    {
    }

    if (ast_nt == arithmeticOrBooleanExpr2)
    {
    }

    if (ast_nt == AnyTerm)
    {
    }

    if (ast_nt == AnyTerm2)
    {
    }

    if (ast_nt == arithmeticExpr)
    {
    }

    if (ast_nt == arithmeticExpr2)
    {
    }

    if (ast_nt == term)
    {
    }

    if (ast_nt == term2)
    {
    }

    if (ast_nt == factor)
    {
    }

/*
 *    if (ast_nt == op1)
 *    {
 *    }
 *
 *    if (ast_nt == op2)
 *    {
 *    }
 */

	if (ast_nt == logicalOp || ast_nt == relationalOp || ast_nt == op1 || ast_nt == op2) {
		tree_node *left_operand = get_child(astn, 0);
		ast_leaf *left_op_data = (ast_leaf *) get_data(left_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!left_op_data->is_leaf) symbol_table_fill(main_st, left_operand, curr_scope);
		else printf("left operand\n");

		ast_leaf *op_node = (ast_leaf *) get_data(get_child(astn, 1));
		printf("operator found here\n");
		
		tree_node *right_operand = get_child(astn, 2);
		ast_leaf *right_op_data = (ast_leaf *) get_data(right_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!right_op_data->is_leaf) symbol_table_fill(main_st, right_operand, curr_scope);
		else printf("right operand\n");

		printf("left: %d; right: %d\n", left_op_data->type, right_op_data->type);
		if (left_op_data->type != right_op_data->type) {
			printf("type mismatch in expr\n");
			// TODO:
		}
		else {
			ast_node *astn_data = (ast_node *) get_data(astn);
			astn_data->type = left_op_data->type;
		}
	}

	/*
     *if (ast_nt == relationalOp)
     *{
     *}
	 */

	if (ast_nt == declareStmt) {
		linked_list *id_ll = ((ast_node *) get_data(get_child(astn, 0)))->ll;
		tree_node *dtype_node = get_child(astn, 1);
		id_type dtype = get_type_from_node(dtype_node);

		int cnt = id_ll->num_nodes;
		for (int i = 0; i < cnt; i++) {
			tree_node *id_lnode = (tree_node *) ll_get(id_ll, i);
			char *id_var_name = ((ast_leaf *) get_data(id_lnode))->ltk->lexeme;
			common_id_entry *id_var_entry = find_id_for_decl(id_var_name, curr_scope);
			if (id_var_entry != NULL) {
				// TODO: redeclaration error 
			}
			else {
				if (dtype == array) {
					tree_node *range_arrays = get_child(dtype_node, 0);
					int *range_indices = get_range_from_node(range_arrays);
					
					arr_id_entry *entry = create_arr_entry(
							id_var_name,
							get_type_from_node(get_child(dtype_node, 1)),
							range_indices[0],
							range_indices[1],
							-1, -1);
					add_to_hash_map(curr_scope->arr_st, id_var_name, entry);
				}
				else {
					var_id_entry *entry = create_var_entry(id_var_name, dtype, -1, -1);
					add_to_hash_map(curr_scope->var_id_st, id_var_name, entry);
				}
			}
		}
	}

	if (ast_nt == condionalStmt) {
		tree_node *id_node = get_child(astn, 0);
		tree_node *casestmt_node = get_child(astn, 1);
		tree_node *default_node = get_child(astn, 2);
		linked_list *casestmts_ll = ((ast_node *) get_data(casestmt_node))->ll;

		// using line_num to generate key for hash map
		int line_num = ((ast_leaf *) get_data(id_node))->ltk->line_num;
		char str_line_num[25];
		sprintf(str_line_num, "%d", line_num);
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		id_type var_type = get_type_from_node(id_node);
		if (var_type == real || var_type == array) {
			// TODO: type err switch var cant be these
		}
		else if (var_type == integer) {
			for (int i = 0; i < casestmts_ll->num_nodes; i++) {
				tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
				tree_node *val_node = get_child(cstmt_node, 0);
				tree_node *stmts_node = get_child(cstmt_node, 1);

				id_type val_type = get_type_from_node(val_node); // can only boolean or integer
																// because of grammar
				if (val_type != integer) {
					// TODO: type error, should be integer
				}

				linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
				for (int j = 0; j < stmts_ll->num_nodes; j++)
					symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
			}

			if (get_data(default_node) == NULL) {
				// TODO: err required default stmt
			}
			else {

			}
		}
		else if (var_type == boolean) {
			if (casestmts_ll->num_nodes != 2) {
				// TODO: err need exactly 2 case stmts (true/false)
			}

			bool got_true = false, got_false = false;
			for (int i = 0; i < 2 && i < casestmts_ll->num_nodes; i++) {
				tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
				tree_node *val_node = get_child(cstmt_node, 0);
				tree_node *stmts_node = get_child(cstmt_node, 1);

				id_type val_type = get_type_from_node(val_node); // can only boolean or integer
																// because of grammar
				if (val_type != boolean) {
					// TODO: type error, should be integer
				}

				terminal val_term = ((ast_leaf *) get_data(val_node))->label.gms.t;
				got_true |= (val_term == TRUE);
				got_false |= (val_term == FALSE);

				linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
				for (int j = 0; j < stmts_ll->num_nodes; j++)
					symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
			}

			if (!got_true || !got_false) {
				// TODO: true false both cases required
			}

			if (get_data(default_node) != NULL) {
				// TODO: err should not have default stmt
			}
		}
		else {
			// TODO: unknown err
		}

		//
		// code gen
		//
	}

	if (ast_nt == for_loop) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = get_data(id_node);
		tree_node *range_node = get_child(astn, 1);
		tree_node *stmts_node = get_child(astn, 2);

		// new scope
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func);

		// insert loop var in scope
		var_id_entry *entry = create_var_entry(id_data->ltk->lexeme, integer, -1, -1);
		add_to_hash_map(new_scope->var_id_st, id_data->ltk->lexeme, entry);
		new_scope->loop_var_entry = entry;

		// using line_num to generate key for hash map
		int line_num = id_data->ltk->line_num;
		char str_line_num[25];
		sprintf(str_line_num, "%d", line_num);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		int *range_indices = get_range_from_node(range_node);
		// TODO: what to do with range here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
	}

	if (ast_nt == while_loop) {
		ast_leaf *while_data = (ast_leaf *) get_data(get_child(astn, 0));
		tree_node *aobexpr_node = get_child(astn, 0);
		tree_node *stmts_node = get_child(astn, 1);

		// using line_num to generate key for hash map
		int line_num = while_data->ltk->line_num;
		char str_line_num[25];
		sprintf(str_line_num, "%d", line_num);
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		// TODO: what to do with aobexpr_node here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
	}
}

void create_symbol_table (tree *ast) {
	tree_node *astn = get_root(ast);
	hash_map *main_st = create_hash_map(DEFAULT_ST_SIZE);

	// default entry for driver module
	func_entry *st_entry = create_func_entry("program", true, false, -1, -1);
	add_to_hash_map(main_st, st_entry->name, st_entry);

	symbol_table_fill (main_st, astn, NULL);
}
