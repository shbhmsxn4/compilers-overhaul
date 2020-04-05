#include "./symbolTableDef.h"

id_type terminal_to_type (terminal t) {
	switch (t) {
		case INTEGER : return integer;
		case REAL : return real;
		case BOOLEAN : return boolean;
		case ARRAY : return array;
		default : return -1;
	}
}

id_type get_type_from_node (tree_node *node) {
	return terminal_to_type(
			((ast_node *) get_data(
				node))->label.gms.t);
}

int* get_range_from_node (tree_node *node) {
	int *indices = (int *) malloc(2*sizeof(int));
	tree_node *range_arrays = get_child(node, 0);
	indices[0] = ((ast_leaf *) get_data(
				get_child(node, 0)))->ltk->nv.int_val;
	indices[1] = ((ast_leaf *) get_data(
				get_child(node, 1)))->ltk->nv.int_val;
	return indices;
}

void symbol_table_fill (hash_map *main_st, tree_node *astn) {
	ast_node* ast_data = (ast_node *)get_data(astn);
	gm_unit data_label = ast_data->label;
	
	assert(!data_label.is_terminal, "ast node for non terminal");
	int num_children = get_num_children(astn);

	nonterminal ast_nt = data_label.gms.nt;

    if (ast_nt == program) {
		symbol_table_fill(main_st, get_child(astn, 0));
		symbol_table_fill(main_st, get_child(astn, 1));
		symbol_table_fill(main_st, get_child(astn, 2));
		symbol_table_fill(main_st, get_child(astn, 3));
    }

    if (ast_nt == moduleDeclarations) {
		linked_list *module_decs = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < module_decs->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(module_decs, i));
    }

	if (ast_nt == moduleDeclaration) {
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "moduleDeclaration : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = (func_entry *) malloc(sizeof(func_entry));
			f_st_entry->only_declared = true;
			f_st_entry->is_called = false;
			strcpy(f_st_entry->name, func_name);
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
			symbol_table_fill(main_st, ll_get(other_mods, i));
	}

	if (ast_nt == driverModule) {
		func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
		assert(st_entry != NULL, "default entry for driver module not found");
		st_entry->only_declared = false;
		//
		//
	}

	if (ast_nt == module) {
		// MODULE ID
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "module : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = (func_entry *) malloc(sizeof(func_entry));
			f_st_entry->only_declared = false;
			f_st_entry->is_called = false;
			strcpy(f_st_entry->name, func_name);
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
				fparam->param.arr_entry->range_start = range_indices[0];
				fparam->param.arr_entry->range_end = range_indices[1];

				fparam->param.arr_entry = (arr_id_entry *) malloc(sizeof(arr_id_entry));
				fparam->param.arr_entry->type = get_type_from_node(get_child(param_type, 1));
				strcpy(fparam->param.arr_entry->lexeme, param_id->ltk->lexeme);
			}
			else {
				fparam->is_array = false;
				fparam->param.var_entry = (var_id_entry *) malloc(sizeof(var_id_entry));
				strcpy(fparam->param.var_entry->lexeme, param_id->ltk->lexeme);
				fparam->param.var_entry->type = type_param;
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
			fparam->param.var_entry = (var_id_entry *) malloc(sizeof(var_id_entry));
			strcpy(fparam->param.var_entry->lexeme, param_id->ltk->lexeme);
			fparam->param.var_entry->type = type_param;

			ll_append(fop_ll, fparam);
		}
		f_st_entry->output_param_list = fop_ll;

		// MODULEDEF
		// ?????????????????
	}

/*
 *    if (ast_nt == ret)
 *    {
 *    }
 *
 *    if (ast_nt == input_plist)
 *    {
 *    }
 *
 *    if (ast_nt == input_plist2)
 *    {
 *    }
 *
 *    if (ast_nt == output_plist)
 *    {
 *    }
 *
 *    if (ast_nt == output_plist2)
 *    {
 *    }
 *
 */
/*
 *    if (ast_nt == dataType)
 *    {
 *    }
 *
 *    if (ast_nt == range_arrays)
 *    {
 *    }
 *
 *    if (ast_nt == type)
 *    {
 *    }
 */

	if (ast_nt == moduleDef) {
		linked_list *statements = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < statments->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(statements, i));
	}

/*
 *    if (ast_nt == statements)
 *    {
 *    }
 *
 *    if (ast_nt == statement)
 *    {
 *    }
 */

    if (ast_nt == ioStmt)
    {
    }

    if (ast_nt == boolConstt)
    {
    }

    if (ast_nt == var_id_num)
    {
    }

    if (ast_nt == var)
    {
    }

    if (ast_nt == whichId)
    {
    }

    if (ast_nt == simpleStmt)
    {
    }

    if (ast_nt == assignmentStmt)
    {
    }

    if (ast_nt == whichStmt)
    {
    }

    if (ast_nt == lvalueIDStmt)
    {
    }

    if (ast_nt == lvalueARRStmt)
    {
    }

    if (ast_nt == index_nt)
    {
    }

    if (ast_nt == moduleReuseStmt)
    {
    }

    if (ast_nt == optional)
    {
    }

    if (ast_nt == idList)
    {
    }

    if (ast_nt == idList2)
    {
    }

    if (ast_nt == expression)
    {
    }

    if (ast_nt == unary_nt)
    {
    }

    if (ast_nt == new_NT)
    {
    }

    if (ast_nt == unary_op)
    {
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

    if (ast_nt == op1)
    {
    }

    if (ast_nt == op2)
    {
    }

    if (ast_nt == logicalOp)
    {
    }

    if (ast_nt == relationalOp)
    {
    }

    if (ast_nt == declareStmt)
    {
    }

    if (ast_nt == condionalStmt)
    {
    }

    if (ast_nt == caseStmts)
    {
    }

    if (ast_nt == caseStmts2)
    {
    }

    if (ast_nt == value)
    {
    }

    if (ast_nt == default_nt)
    {
    }

    if (ast_nt == iterativeStmt)
    {
    }

    if (ast_nt == range)
    {
    }

}

void create_symbol_table (tree_node *astn) {
	hash_map *main_st = create_hash_map(71);

	// default entry for driver module
	func_entry *st_entry = (func_entry *) malloc(sizeof(func_entry));
	st_entry->only_declared = true;
	strcpy(st_entry->name, "program");
	add_to_hash_map(main_st, st_entry->name, st_entry);

	symbol_table_fill (main_st, astn);
}
