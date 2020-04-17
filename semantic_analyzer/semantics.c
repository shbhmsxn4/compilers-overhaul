#include "../symbol_table/symbol_table.h"
#include "../utils/terminal_name.h"


// FIRST AST PASS
void first_pass (hash_map *main_st, tree_node *astn, scope_node *curr_scope) {
	ast_node* ast_data = (ast_node *)get_data(astn);
	gm_unit data_label = ast_data->label;
	
	int num_children = get_num_children(astn);

	nonterminal ast_nt = data_label.gms.nt;

	/*
	 *char ast_nt_name[100];
	 *nonterminal_name(ast_nt, ast_nt_name);
	 *printf("here %s\n", ast_nt_name);
	 */
    if (ast_nt == program) {
		first_pass(main_st, get_child(astn, 0), curr_scope);
		first_pass(main_st, get_child(astn, 1), curr_scope);

		// driver module
		func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
		assert(st_entry != NULL, "default entry for driver module not found");
		first_pass(main_st, get_child(astn, 2), st_entry->local_scope);

		first_pass(main_st, get_child(astn, 3), curr_scope);
    }

    if (ast_nt == moduleDeclarations) {
		linked_list *module_decs = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < module_decs->num_nodes; i++)
			first_pass(main_st, ll_get(module_decs, i), curr_scope);
    }

	if (ast_nt == moduleDeclaration) {
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "moduleDeclaration : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, true, false, false, -1, 0);
			add_to_hash_map(main_st, func_name, f_st_entry);
		}
		else {
			char err_msg[100];
			sprintf(err_msg, "'%s' module re-declared", func_name);
			display_err("Semantic", module_id_data->ltk->line_num, err_msg);
		}
		//
		//
	}

	if (ast_nt == otherModules) {
		linked_list *other_mods = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < other_mods->num_nodes; i++)
			first_pass(main_st, ll_get(other_mods, i), curr_scope);
	}

	if (ast_nt == module) {
		// MODULE ID
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "module : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		bool entry_exists = true;
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, false, true, false, -1, 0);
			entry_exists = false;
			/*add_to_hash_map(main_st, func_name, f_st_entry);*/
		}
		else {
			if (f_st_entry->is_defined) {
				char err_msg[100];
				sprintf(err_msg, "'%s' module re-defined", func_name);
				display_err("Semantic", module_id_data->ltk->line_num, err_msg);
				return; // don't process the module
			}
			if (f_st_entry->is_declared && !f_st_entry->is_called) {
				char err_msg[100];
				sprintf(err_msg, "'%s' module declaration & definition appear before its call", func_name);
				display_err("Semantic", module_id_data->ltk->line_num, err_msg);
			}
		}

		f_st_entry->is_defined = true;
		
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
				int *range_indices = get_static_range(range_arrays);
				// TODO: module params dont have dynamic ranges?
				/*common_id_entry **range_entries = get_dynamic_range(range_arrays, NULL);*/

				fparam->param.arr_entry = create_arr_entry(
						param_id->ltk->lexeme,
						get_type_from_node(get_child(param_type, 1)),
						range_indices, NULL,
						-1, -1);

				arr_assign_offset(fparam->param.arr_entry, f_st_entry);
			}
			else {
				fparam->is_array = false;
				fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);

				var_assign_offset(fparam->param.var_entry, f_st_entry);
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

			fparam->is_array = false; // output param cannot be array
			fparam->is_assigned = false;
			fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);

			var_assign_offset(fparam->param.var_entry, f_st_entry);

			// TEMP
			var_id_entry *entry = fparam->param.var_entry;

			ll_append(fop_ll, fparam);
		}
		f_st_entry->output_param_list = fop_ll;

		// NOTE:
		f_st_entry->width = 0; // resetting width for local params

		if (!entry_exists) add_to_hash_map(main_st, func_name, f_st_entry);

		// MODULEDEF
		first_pass(main_st, get_child(astn, 3), f_st_entry->local_scope);

		// check if all output params have been assigned
		for (int i = 0; i < f_st_entry->output_param_list->num_nodes; i++) {
			param_node *pnode = (param_node *) ll_get(fop_ll, i);

			if (!pnode->is_assigned) {
				char *pname;
				if (pnode->is_array) {
					pname = pnode->param.arr_entry->lexeme;
				}
				else {
					pname = pnode->param.var_entry->lexeme;
				}
				char err_msg[100];
				sprintf(err_msg, "output param '%s' is not assigned in module '%s'", pname, func_name);
				display_err("Semantic", f_st_entry->local_scope->end_line, err_msg);
			}
		}
	}

	if (ast_nt == moduleDef) {
		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 0));
		ast_leaf *end_data = (ast_leaf *) get_data(get_child(astn, 2));
		int start_line_num = start_data->ltk->line_num;
		int end_line_num = end_data->ltk->line_num;

		curr_scope->start_line = start_line_num;
		curr_scope->end_line = end_line_num;

		first_pass(main_st, get_child(astn, 1), curr_scope);
	}

	if (ast_nt == statements) {
		linked_list *statements = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < statements->num_nodes; i++) {
			first_pass(main_st, ll_get(statements, i), curr_scope);
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
			ast_leaf *whichid_data = (ast_leaf *) get_data(whichid_node);

			common_id_entry *id_entry = type_check_var(id_data, whichid_data, curr_scope, for_use);
			var_data->type = type_from_entry(id_entry, whichid_data != NULL);
		}
	}

	/*ioStmt start*/
	if (ast_nt == input_stmt) {
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0)); 
		type_check_var(id_data, NULL, curr_scope, for_assign);
		
		// TODO:
		// generate assembly code to take input
		// and store at corresponding offset
		// taken from ST
	}

	if (ast_nt == output_stmt) {
		tree_node *var_node = get_child(astn, 0);
		ast_leaf *var_data = ((ast_leaf *) get_data(var_node));
		if (var_data->is_leaf) {
			// TODO:
		}
		else {
			first_pass(main_st, var_node, curr_scope);
		}
	}
	/*ioStmt end*/

	if (ast_nt == assignmentStmt) {
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0));
		tree_node *whichstmt_node = get_child(astn, 1);
		ast_node *whichstmt_data = (ast_node *) get_data(whichstmt_node);
		
		common_id_entry *id_entry;
		id_type var_type;
		tree_node *expr_node;
		if (whichstmt_data->label.gms.nt == lvalueIDStmt) {
			expr_node = get_child(whichstmt_node, 0);
			id_entry = type_check_var(id_data, NULL, curr_scope, for_assign);
			var_type = type_from_entry(id_entry, false);

			// TODO: generate assembly code to compute expr value
			// and assign it to the id here
		}
		else if (whichstmt_data->label.gms.nt == lvalueARRStmt) {
			tree_node *index_node = get_child(whichstmt_node, 0);
			ast_leaf *index_data = (ast_leaf *) get_data(index_node);
			id_entry = type_check_var(id_data, index_data, curr_scope, for_assign);
			var_type = type_from_entry(id_entry, true);
			expr_node = get_child(whichstmt_node, 1);
		}

		ast_node *expr_data = (ast_node *) get_data(expr_node);
		if (!expr_data->is_leaf) first_pass(main_st, expr_node, curr_scope);

		/*printf("lhs: %d; rhs: %d\n", var_type, expr_data->type);*/
		if (expr_data->type == -1 || var_type == -1) {
			// TODO: means that the rhs had internal type conflcts
			// what to do in this case??
		}
		else if (var_type != expr_data->type) {
			char err_msg[100];
			sprintf(err_msg, "type mismatch - LHS is %s, RHS is %s", type_to_str(var_type), type_to_str(expr_data->type));
			display_err("Type", id_data->ltk->line_num, err_msg);
		}
		else if (expr_data->type == array) {
			ast_leaf *rhs_arr_data = (ast_leaf *) get_data(get_child(expr_node, 0));
			char *rhs_arr_name = rhs_arr_data->ltk->lexeme;
			common_id_entry *rhs_entry = type_check_var(rhs_arr_data, NULL, curr_scope, for_use);
			if (!is_same_type(id_entry, rhs_entry)) {
				char err_msg[100];
				sprintf(err_msg, "type mismatch in LHS, RHS arrays");
				display_err("Type", id_data->ltk->line_num, err_msg);
			}
		}
	}

	if (ast_nt == moduleReuseStmt) {
		tree_node *opt_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 1));

		char *func_name = id_data->ltk->lexeme;
		func_entry *fentry = fetch_from_hash_map(main_st, func_name);

		if (fentry == NULL) {
			char err_msg[100];
			sprintf(err_msg, "'%s' module is unknown", func_name);
			display_err("Semantic", id_data->ltk->line_num, err_msg);
			return;
		}
		else {
			if (fentry == curr_scope->func) {
				char err_msg[100];
				sprintf(err_msg, "'%s' module recursion not allowed", func_name);
				display_err("Semantic", id_data->ltk->line_num, err_msg);
			}

			fentry->is_called = true;
		}

		// OUTPUT PARAMS - return values
/*
 *        if (get_data(opt_node) != NULL) {
 *            linked_list *ret_var_ll = ((ast_node *) get_data(opt_node))->ll;
 *            linked_list *op_ll = fentry->output_param_list;
 *
 *            if (ret_var_ll->num_nodes != op_ll->num_nodes) {
 *                // TODO: params count mismatch err
 *            }
 *            else {
 *                int pcnt = op_ll->num_nodes;
 *                for (int i = 0; i < pcnt; i++) {
 *                    tree_node *ret_var_lnode = (tree_node *) ll_get(ret_var_ll, i);
 *                    ast_leaf *ret_var_data = (ast_leaf *) get_data(ret_var_lnode);
 *                    char *ret_var_name = ret_var_data->ltk->lexeme;
 *                    common_id_entry *ret_var_entry = type_check_var(ret_var_data, NULL, curr_scope, for_assign);
 *                    if (ret_var_entry == NULL) {
 *                        printf("undeclared var : %s\n", ret_var_name);
 *                        // TODO: undeclared var
 *                    }
 *                    else {
 *                        param_node *op_lnode = (param_node *) ll_get(op_ll, i);
 *                        common_id_entry *op_lentry = param_to_st_entry(op_lnode);
 *                        if (!is_same_type(ret_var_entry, op_lentry)) {
 *                            printf("type mismatch for output params : %s\n", ret_var_name);
 *                            // TODO: type mismatch between function call and assign
 *                        }
 *                    }
 *                }
 *            }
 *        }
 *
 *        // INPUT PARAMS - args
 *        linked_list *arg_var_ll = ((ast_node *) get_data(get_child(astn, 2)))->ll;
 *        linked_list *ip_ll = fentry->input_param_list;
 *
 *        if (arg_var_ll->num_nodes != ip_ll->num_nodes) {
 *            // TODO: params count mismatch err
 *        }
 *        else {
 *            int pcnt = ip_ll->num_nodes;
 *            for (int i = 0; i < pcnt; i++) {
 *                tree_node *arg_var_lnode = (tree_node *) ll_get(arg_var_ll, i);
 *                ast_leaf *arg_var_data = (ast_leaf *) get_data(arg_var_lnode);
 *                char *arg_var_name = arg_var_data->ltk->lexeme;
 *                common_id_entry *arg_var_entry = type_check_var(arg_var_data, NULL, curr_scope, for_use);
 *                if (arg_var_entry == NULL) {
 *                    printf("undeclared var : %s\n", arg_var_name);
 *                    // TODO: undeclared var
 *                }
 *                else {
 *                    param_node *ip_lnode = (param_node *) ll_get(ip_ll, i);
 *                    common_id_entry *ip_lentry = param_to_st_entry(ip_lnode);
 *                    if (!is_same_type(arg_var_entry, ip_lentry)) {
 *                        printf("type mismatch for input params : %s\n", arg_var_name);
 *                        // TODO: type mismatch between function call and assign
 *                    }
 *                }
 *            }
 *        }
 */
	}
    
    if (ast_nt == unary_nt) {
		ast_leaf *uop = (ast_leaf *) get_data(get_child(astn, 0));
		/*printf("unary operator : %s\n", uop->ltk->lexeme);*/
		tree_node *expr_node = get_child(astn, 1);
		// TODO: what to do with the unary operator here??
		first_pass(main_st, expr_node, curr_scope);

		ast_node *expr_data = (ast_node *) get_data(expr_node);
		if (expr_data->type != -1 && expr_data->type != integer && expr_data->type != real) {
			char err_msg[100];
			sprintf(err_msg, "Expression after unary operator should be integer or real, found %s", type_to_str(expr_data->type));
			display_err("Type", uop->ltk->line_num, err_msg);
		}
		else {
			((ast_node *) (get_data(astn)))->type = expr_data->type;
		}
	}

	if (ast_nt == logicalOp || ast_nt == relationalOp || ast_nt == op1 || ast_nt == op2) {
		tree_node *left_operand = get_child(astn, 0);
		ast_leaf *left_op_data = (ast_leaf *) get_data(left_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!left_op_data->is_leaf) first_pass(main_st, left_operand, curr_scope);
		/*else printf("left operand\n");*/

		ast_leaf *op_node = (ast_leaf *) get_data(get_child(astn, 1));
		/*printf("operator found here\n");*/
		
		tree_node *right_operand = get_child(astn, 2);
		ast_leaf *right_op_data = (ast_leaf *) get_data(right_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!right_op_data->is_leaf) first_pass(main_st, right_operand, curr_scope);
		/*else printf("right operand\n");*/

		ast_node *astn_data = (ast_node *) get_data(astn);

		/*printf("left: %d; right: %d\n", left_op_data->type, right_op_data->type);*/
		if (left_op_data->type == array || right_op_data->type == array) {
			char err_msg[100];
			char op_name[10];
			terminal_name(op_node->label.gms.t, op_name);
			sprintf(err_msg, "array(s) cannot be involved expression(s) at operator '%s'", op_name);
			display_err("Type", op_node->ltk->line_num, err_msg);
		}
		else if (ast_nt == logicalOp) {
			char err_msg[150];
			char op_name[10];
			terminal_name(op_node->label.gms.t, op_name);
			if (left_op_data->type == -1 || right_op_data->type == -1) {
				// TODO: WHAT HERE?
			}
			else if (left_op_data->type != right_op_data->type) {
				sprintf(err_msg, "type mismatch at operator '%s', left operand is %s right operand is %s", op_name, type_to_str(left_op_data->type), type_to_str(right_op_data->type));
				display_err("Type", op_node->ltk->line_num, err_msg);
			}
			else if (left_op_data->type != boolean) {
				sprintf(err_msg, "Incorrect types at operator '%s', left and right operands should be boolean, found %s", op_name, type_to_str(left_op_data->type));
				display_err("Type", op_node->ltk->line_num, err_msg);
			}
			else {
				astn_data->type = boolean;
			}
		}
		else {
			char err_msg[150];
			char op_name[10];
			terminal_name(op_node->label.gms.t, op_name);
			if (left_op_data->type == -1 || right_op_data->type == -1) {
				// TODO: WHAT HERE?
			}
			else if (left_op_data->type != right_op_data->type) {
				sprintf(err_msg, "type mismatch at operator '%s', left operand is %s right operand is %s", op_name, type_to_str(left_op_data->type), type_to_str(right_op_data->type));
				display_err("Type", op_node->ltk->line_num, err_msg);
			}
			else if (left_op_data->type == boolean) {
				sprintf(err_msg, "Incorrect types at operator '%s', left and right operands should not be boolean", op_name);
				display_err("Type", op_node->ltk->line_num, err_msg);
			}
			else if (ast_nt == relationalOp) {
				astn_data->type = boolean;
			}
			else {
				astn_data->type = left_op_data->type;
			}
		}
		
	}

	if (ast_nt == declareStmt) {
		linked_list *id_ll = ((ast_node *) get_data(get_child(astn, 0)))->ll;
		tree_node *dtype_node = get_child(astn, 1);
		id_type dtype = get_type_from_node(dtype_node);

		int cnt = id_ll->num_nodes;
		for (int i = 0; i < cnt; i++) {
			tree_node *id_lnode = (tree_node *) ll_get(id_ll, i);
			ast_leaf *id_data = (ast_leaf *) get_data(id_lnode);
			char *id_var_name = id_data->ltk->lexeme;
			common_id_entry *id_var_entry = find_id_for_decl(id_var_name, curr_scope, id_data->ltk->line_num);
			if (id_var_entry != NULL) {
				char err_msg[100];
				sprintf(err_msg, "'%s' variable re-declared", id_var_name);
				display_err("Semantic", id_data->ltk->line_num, err_msg);
			}
			else {
				if (dtype == array) {
					tree_node *range_arrays = get_child(dtype_node, 0);
					int *range_indices = get_static_range(range_arrays);
					/*common_id_entry **range_entries = get_dynamic_range(range_arrays, curr_scope);*/
					char **range_lexemes = get_dynamic_range(range_arrays, curr_scope);
					
					arr_id_entry *entry = create_arr_entry(
							id_var_name,
							get_type_from_node(get_child(dtype_node, 1)),
							range_indices, range_lexemes,
							-1, -1);

					arr_assign_offset(entry, curr_scope->func);

					add_to_hash_map(curr_scope->arr_st, id_var_name, entry);
				}
				else {
					var_id_entry *entry = create_var_entry(id_var_name, dtype, -1, -1);

					var_assign_offset(entry, curr_scope->func);

					add_to_hash_map(curr_scope->var_id_st, id_var_name, entry);
				}
			}
		}
	}

	if (ast_nt == condionalStmt) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(id_node);
		tree_node *casestmt_node = get_child(astn, 2);
		tree_node *default_node = get_child(astn, 3);
		ast_leaf *default_data = (ast_leaf *) get_data(default_node);
		linked_list *casestmts_ll = ((ast_node *) get_data(casestmt_node))->ll;

		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 1));
		ast_leaf *end_data = (ast_leaf *) get_data(get_child(astn, 4));
		int start_line_num = start_data->ltk->line_num;
		int end_line_num = end_data->ltk->line_num;

		// using line_num to generate key for hash map
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", start_line_num);
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func, start_line_num, end_line_num);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		common_id_entry *entry = type_check_var(id_data, NULL, new_scope, for_use);
		id_type var_type = type_from_entry(entry, false);
		if (var_type == real || var_type == array) {
			char err_msg[100];
			sprintf(err_msg, "switch var '%s' should be integer or boolean, found %s", id_data->ltk->lexeme, type_to_str(var_type));
			display_err("Semantic", id_data->ltk->line_num, err_msg);
			return;
		}

		int casestmts_cnt = casestmts_ll->num_nodes;

		bool got_true = false, got_false = false;
		for (int i = 0; i < casestmts_cnt; i++) {
			tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
			tree_node *val_node = get_child(cstmt_node, 0);
			ast_leaf *val_data = (ast_leaf *) get_data(val_node);
			tree_node *stmts_node = get_child(cstmt_node, 1);

			id_type val_type = get_type_from_node(val_node); // can only boolean or integer
															// because of grammar
			if (val_type != var_type) {
				char err_msg[100];
				sprintf(err_msg, "found case '%s' of type %s for switch of var '%s' (%s)", val_data->ltk->lexeme, type_to_str(val_type), id_data->ltk->lexeme, type_to_str(var_type));
				display_err("Type", val_data->ltk->line_num, err_msg);
			}
			else if (val_type == boolean) {
				terminal val_term = ((ast_leaf *) get_data(val_node))->label.gms.t;
				got_true |= (val_term == TRUE);
				got_false |= (val_term == FALSE);
			}

			linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
			for (int j = 0; j < stmts_ll->num_nodes; j++)
				first_pass(main_st, ll_get(stmts_ll, j), new_scope);
		}

		if (var_type == integer) {
			if (default_data == NULL) {
				char err_msg[100];
				sprintf(err_msg, "switch of var '%s' (integer) should have a default statement", id_data->ltk->lexeme);
				display_err("Semantic", end_line_num, err_msg);
			}
		}
		else if (var_type == boolean) {
			char err_msg[100];
			if (!got_true) {
				sprintf(err_msg, "case 'true' not found for switch of var '%s' (boolean)", id_data->ltk->lexeme);
				display_err("Semantic", end_line_num, err_msg);
			}
			if (!got_false) {
				sprintf(err_msg, "case 'false' not found for switch of var '%s' (boolean)", id_data->ltk->lexeme);
				display_err("Semantic", end_line_num, err_msg);
			}
			if (default_data != NULL) {
				sprintf(err_msg, "switch of var '%s' (boolean) should not have a default statement", id_data->ltk->lexeme);
				display_err("Semantic", ((ast_leaf *) get_data(get_child(default_node, 0)))->ltk->line_num, err_msg);
			}
		}

		if (var_type == boolean && casestmts_cnt != 2) {
							char err_msg[100];
							sprintf(err_msg, "switch of var '%s' (boolean) should have exactly 2 case statements, found %d", id_data->ltk->lexeme, casestmts_cnt);
							display_err("Semantic", end_line_num, err_msg);
		}

		if (default_data != NULL) {
			tree_node *stmts_node = get_child(default_node, 1);

			linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
			for (int j = 0; j < stmts_ll->num_nodes; j++)
				first_pass(main_st, ll_get(stmts_ll, j), new_scope);
		}

		//
		// code gen
		//
	}

	if (ast_nt == for_loop) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = get_data(id_node);
		tree_node *range_node = get_child(astn, 1);
		tree_node *stmts_node = get_child(astn, 3);

		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 2));
		ast_leaf *end_data = (ast_leaf *) get_data(get_child(astn, 4));
		int start_line_num = start_data->ltk->line_num;
		int end_line_num = end_data->ltk->line_num;

		// new scope
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func, start_line_num, end_line_num);

		// insert loop var in scope
		var_id_entry *entry = create_var_entry(id_data->ltk->lexeme, integer, -1, -1);
		add_to_hash_map(new_scope->var_id_st, id_data->ltk->lexeme, entry);
		new_scope->loop_var_entry = entry;

		var_assign_offset(entry, curr_scope->func);

		// using start line num to generate key for hash map
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", start_line_num);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		int *range_indices = get_static_range(range_node);
		// NO dynamic range possible, bcuz of grammar rules
		// TODO: what to do with range here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			first_pass(main_st, ll_get(stmts_ll, j), new_scope);
	}

	if (ast_nt == while_loop) {
		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 1));
		ast_leaf *end_data = (ast_leaf *) get_data(get_child(astn, 3));
		tree_node *aobexpr_node = get_child(astn, 0);
		tree_node *stmts_node = get_child(astn, 2);

		first_pass(main_st, aobexpr_node, curr_scope);

				// using start line num to generate key for hash map
		int start_line_num = start_data->ltk->line_num;
		int end_line_num = end_data->ltk->line_num;
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", start_line_num);
		scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func, start_line_num, end_line_num);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		ast_node *aobexpr_data = (ast_node *) get_data(aobexpr_node);
		if (aobexpr_data->type != -1 && aobexpr_data->type != boolean) {
			char err_msg[100];
			sprintf(err_msg, "gaurd condition for while should be boolean type, found %s instead", type_to_str(aobexpr_data->type));
			display_err("Type", start_line_num, err_msg);
		}

		// TODO: what to do with aobexpr_node here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			first_pass(main_st, ll_get(stmts_ll, j), new_scope);
	}
}

void second_pass (hash_map *main_st, tree_node *astn, scope_node *curr_scope) {
	ast_node* ast_data = (ast_node *)get_data(astn);
	gm_unit data_label = ast_data->label;
	
	int num_children = get_num_children(astn);

	nonterminal ast_nt = data_label.gms.nt;

	/*
	 *char ast_nt_name[100];
	 *nonterminal_name(ast_nt, ast_nt_name);
	 *printf("pass 2 here %s\n", ast_nt_name);
	 */
    if (ast_nt == program) {
		second_pass(main_st, get_child(astn, 0), curr_scope);
		second_pass(main_st, get_child(astn, 1), curr_scope);

		// driver module
		func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
		assert(st_entry != NULL, "default entry for driver module not found");
		second_pass(main_st, get_child(astn, 2), st_entry->local_scope);

		second_pass(main_st, get_child(astn, 3), curr_scope);
    }

    if (ast_nt == moduleDeclarations) {
		linked_list *module_decs = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < module_decs->num_nodes; i++)
			second_pass(main_st, ll_get(module_decs, i), curr_scope);
    }

	if (ast_nt == moduleDeclaration) {
/*
 *        tree_node* module_id_node = get_child(astn, 0);
 *        ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);
 *
 *        assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "moduleDeclaration : need ID here");
 *
 *        char *func_name = module_id_data->ltk->lexeme;
 *        func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
 *        if (f_st_entry == NULL) {
 *            f_st_entry = create_func_entry(func_name, true, false, false, -1, -1);
 *            add_to_hash_map(main_st, func_name, f_st_entry);
 *        }
 *        else {
 *            // TODO: module redeclaration error
 *        }
 *        //
 *        //
 */
	}

	if (ast_nt == otherModules) {
		linked_list *other_mods = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < other_mods->num_nodes; i++)
			second_pass(main_st, ll_get(other_mods, i), curr_scope);
	}

	if (ast_nt == module) {
		// MODULE ID
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "module : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
/*
 *        if (f_st_entry == NULL) {
 *            f_st_entry = create_func_entry(func_name, false, true, false, -1, -1);
 *            [>add_to_hash_map(main_st, func_name, f_st_entry);<]
 *        }
 *        else {
 *            if (f_st_entry->is_defined) {
 *                printf("multiple definitions for function : %s\n", func_name);
 *                // TODO:
 *            }
 *            if (f_st_entry->is_declared && !f_st_entry->is_called) {
 *                printf("redundant function declaration : %s\n", func_name);
 *                // TODO: redundant declaration, only definition required
 *            }
 *        }
 *
 *        f_st_entry->is_defined = true;
 *        
 *        // MODULE INPUT PARAM LIST
 *        tree_node* module_iplist = get_child(astn, 1);
 *        ast_node* module_iplist_data = (ast_node *) get_data(module_iplist);
 *
 *        assert(!module_iplist_data->label.is_terminal && module_iplist_data->label.gms.nt == input_plist, "module : need input_plist here");
 *
 *        linked_list *ip_ll = module_iplist_data->ll;
 *        linked_list *fip_ll = create_linked_list();
 *        for (int i = 0; i < ip_ll->num_nodes; i++) {
 *            param_node *fparam = (param_node *) malloc(sizeof(param_node));
 *            tree_node *param = (tree_node *) ll_get(ip_ll, i);
 *            ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
 *            tree_node *param_type = get_child(param, 1);
 *            id_type type_param = get_type_from_node(param_type);
 *
 *            if (type_param == array) {
 *                fparam->is_array = true;
 *                
 *                tree_node *range_arrays = get_child(param_type, 0);
 *                int *range_indices = get_range_from_node(range_arrays);
 *
 *                fparam->param.arr_entry = create_arr_entry(
 *                        param_id->ltk->lexeme,
 *                        get_type_from_node(get_child(param_type, 1)),
 *                        range_indices[0],
 *                        range_indices[1],
 *                        -1, -1);
 *            }
 *            else {
 *                fparam->is_array = false;
 *                fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);
 *            }
 *            ll_append(fip_ll, fparam);
 *        }
 *        f_st_entry->input_param_list = fip_ll;
 *
 *        // MODULE OUTPUT PARAM LIST
 *        tree_node* module_oplist = get_child(astn, 2);
 *        ast_node* module_oplist_data = (ast_node *) get_data(module_oplist);
 *
 *        assert(!module_oplist_data->label.is_terminal && module_oplist_data->label.gms.nt == output_plist, "module : need output_plist here");
 *
 *        linked_list *op_ll = module_oplist_data->ll;
 *        linked_list *fop_ll = create_linked_list();
 *        for (int i = 0; i < op_ll->num_nodes; i++) {
 *            param_node *fparam = (param_node *) malloc(sizeof(param_node));
 *            tree_node *param = (tree_node *) ll_get(op_ll, i);
 *            ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
 *            tree_node *param_type = get_child(param, 1);
 *            id_type type_param = get_type_from_node(param_type);
 *
 *            fparam->is_array = false; // output param cannot be array
 *            fparam->is_assigned = false;
 *            fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);
 *
 *            ll_append(fop_ll, fparam);
 *        }
 *        f_st_entry->output_param_list = fop_ll;
 *
 *        add_to_hash_map(main_st, func_name, f_st_entry);
 */

		if (f_st_entry != NULL) {
			// MODULEDEF
			second_pass(main_st, get_child(astn, 3), f_st_entry->local_scope);
		}

		// check if all output params have been assigned
		/*
		 *for (int i = 0; i < f_st_entry->output_param_list->num_nodes; i++) {
		 *    param_node *pnode = (param_node *) ll_get(fop_ll, i);
		 *    if (!pnode->is_assigned) {
		 *        printf("output params need to be assigned value\n");
		 *        // TODO: err unassigned output param
		 *    }
		 *}
		 */
	}

	if (ast_nt == moduleDef) {
		second_pass(main_st, get_child(astn, 1), curr_scope);
	}

	if (ast_nt == statements) {
		linked_list *statements = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < statements->num_nodes; i++) {
			second_pass(main_st, ll_get(statements, i), curr_scope);
		}
	}

	if (ast_nt == var_id_num) {
/*
 *        int child_cnt = get_num_children(astn);
 *        tree_node *var_node = astn;
 *        ast_node *var_data = (ast_node *) get_data(astn);
 *        if (child_cnt == 1) {
 *            // VAR -> NUM, RNUM, TRUE, FALSE
 *            tree_node *const_var_node = get_child(var_node, 0);
 *            ast_leaf *const_var_data = (ast_leaf *) get_data(const_var_node);
 *            var_data->type = const_var_data->type;
 *            // TODO: generate ast code to display value of
 *            // this constant
 *        }
 *        else if (child_cnt == 2) {
 *            ast_leaf *id_data = (ast_leaf *) get_data(get_child(var_node, 0));
 *            tree_node *whichid_node = get_child(var_node, 1);
 *            ast_leaf *whichid_data = (ast_leaf *) get_data(whichid_node);
 *
 *            common_id_entry *id_entry = type_check_var(id_data, whichid_data, curr_scope, for_use);
 *            var_data->type = type_from_entry(id_entry, whichid_data != NULL);
 *        }
 */
	}

	/*ioStmt start*/
	if (ast_nt == input_stmt) {
		/*
		 *ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0)); 
		 *type_check_var(id_data, NULL, curr_scope, for_assign);
		 */
		
		// TODO:
		// generate assembly code to take input
		// and store at corresponding offset
		// taken from ST
	}

	if (ast_nt == output_stmt) {
		tree_node *var_node = get_child(astn, 0);
		
		if (((ast_leaf *) get_data(var_node))->is_leaf) {
			/*printf("output stmt printing const\n");*/
		}
		else {
			second_pass(main_st, var_node, curr_scope);
		}
	}
	/*ioStmt end*/

	if (ast_nt == assignmentStmt) {
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0));
		tree_node *whichstmt_node = get_child(astn, 1);
		ast_node *whichstmt_data = (ast_node *) get_data(whichstmt_node);
		
		common_id_entry *id_entry;
		id_type var_type;
		tree_node *expr_node;
		if (whichstmt_data->label.gms.nt == lvalueIDStmt) {
			expr_node = get_child(whichstmt_node, 0);
			/*
			 *id_entry = type_check_var(id_data, NULL, curr_scope, for_assign);
			 *var_type = type_from_entry(id_entry, false);
			 */

			// TODO: generate assembly code to compute expr value
			// and assign it to the id here
		}
		else if (whichstmt_data->label.gms.nt == lvalueARRStmt) {
			/*
			 *tree_node *index_node = get_child(whichstmt_node, 0);
			 *ast_leaf *index_data = (ast_leaf *) get_data(index_node);
			 *id_entry = type_check_var(id_data, index_data, curr_scope, for_assign);
			 *var_type = type_from_entry(id_entry, true);
			 */
			expr_node = get_child(whichstmt_node, 1);
		}

		ast_node *expr_data = (ast_node *) get_data(expr_node);
		if (!expr_data->is_leaf) second_pass(main_st, expr_node, curr_scope);
/*
 *        ast_node *expr_data = (ast_node *) get_data(expr_node);
 *
 *        printf("lhs: %d; rhs: %d\n", var_type, expr_data->type);
 *        if (expr_data->type == -1) {
 *            // TODO: means that the rhs had internal type conflcts
 *            // what to do in this case??
 *        }
 *
 *        if (var_type != expr_data->type) {
 *            printf("type err : lhs rhs of assignment dont match\n");
 *            // TODO: 
 *        }
 *        else if (expr_data->type == array) {
 *            char *rhs_arr_name = ((ast_leaf *) get_data(get_child(expr_node, 0)))->ltk->lexeme;
 *            common_id_entry *rhs_entry = find_id_for_use(rhs_arr_name, curr_scope);
 *            if (!is_same_type(id_entry, rhs_entry)) {
 *                printf("type err : lhs rhs of assignment dont match (arrays)\n");
 *            }
 *        }
 */
	}

	if (ast_nt == moduleReuseStmt) {
		tree_node *opt_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 1));

		char *func_name = id_data->ltk->lexeme;
		func_entry *fentry = fetch_from_hash_map(main_st, func_name);

		if (fentry == NULL) {
			return;
		}

		fentry->is_called = true;
		/*
		 *if (fentry == curr_scope->func) {
		 *    printf("recursion not allowed\n");
		 *    // TODO: recursion not allowed
		 *}
		 */

		// OUTPUT PARAMS - return values
		if (get_data(opt_node) != NULL) {
			linked_list *ret_var_ll = ((ast_node *) get_data(opt_node))->ll;
			linked_list *op_ll = fentry->output_param_list;

			if (ret_var_ll->num_nodes != op_ll->num_nodes) {
				char err_msg[150];
				sprintf(err_msg, "output params count mismatch - %d in module definition, %d in module reuse", op_ll->num_nodes, ret_var_ll->num_nodes);
				display_err("Semantic", id_data->ltk->line_num, err_msg);
			}
			else {
				int pcnt = op_ll->num_nodes;
				for (int i = 0; i < pcnt; i++) {
					tree_node *ret_var_lnode = (tree_node *) ll_get(ret_var_ll, i);
					ast_leaf *ret_var_data = (ast_leaf *) get_data(ret_var_lnode);
					char *ret_var_name = ret_var_data->ltk->lexeme;
					common_id_entry *ret_var_entry = type_check_var(ret_var_data, NULL, curr_scope, for_assign);
					if (ret_var_entry != NULL) {
						param_node *op_lnode = (param_node *) ll_get(op_ll, i);
						common_id_entry *op_lentry = param_to_st_entry(op_lnode);
						if (!is_same_type(ret_var_entry, op_lentry)) {
							char err_msg[100];
							sprintf(err_msg, "incorrect type for output param '%s' in module reuse", ret_var_name);
							display_err("Semantic", ret_var_data->ltk->line_num, err_msg);
						}
					}
				}
			}
		}

		// INPUT PARAMS - args
		linked_list *arg_var_ll = ((ast_node *) get_data(get_child(astn, 2)))->ll;
		linked_list *ip_ll = fentry->input_param_list;

		if (arg_var_ll->num_nodes != ip_ll->num_nodes) {
			char err_msg[150];
			sprintf(err_msg, "input params count mismatch - %d in module definition, %d in module reuse", ip_ll->num_nodes, arg_var_ll->num_nodes);
			display_err("Semantic", id_data->ltk->line_num, err_msg);
		}
		else {
			int pcnt = ip_ll->num_nodes;
			for (int i = 0; i < pcnt; i++) {
				tree_node *arg_var_lnode = (tree_node *) ll_get(arg_var_ll, i);
				ast_leaf *arg_var_data = (ast_leaf *) get_data(arg_var_lnode);
				char *arg_var_name = arg_var_data->ltk->lexeme;
				common_id_entry *arg_var_entry = type_check_var(arg_var_data, NULL, curr_scope, for_use);
				if (arg_var_entry != NULL) {
					param_node *ip_lnode = (param_node *) ll_get(ip_ll, i);
					common_id_entry *ip_lentry = param_to_st_entry(ip_lnode);
					if (!is_same_type(arg_var_entry, ip_lentry)) {
						char err_msg[100];
						sprintf(err_msg, "incorrect type for input param '%s' in module reuse", arg_var_name);
						display_err("Semantic", arg_var_data->ltk->line_num, err_msg);
					}
				}
			}
		}
	}
    
    if (ast_nt == unary_nt) {
		ast_leaf *uop = (ast_leaf *) get_data(get_child(astn, 0));
		/*printf("unary operator : %s\n", uop->ltk->lexeme);*/
		tree_node *expr_node = get_child(astn, 1);
		// TODO: what to do with the unary operator here??
		second_pass(main_st, expr_node, curr_scope);

		/*
		 *ast_node *expr_data = (ast_node *) get_data(expr_node);
		 *if (expr_data->type != integer && expr_data->type != real) {
		 *    printf("type error for unary : should be integer/real\n");
		 *    // TODO: 
		 *}
		 *else {
		 *    ((ast_node *) (get_data(astn)))->type = expr_data->type;
		 *}
		 */
	}

	if (ast_nt == logicalOp || ast_nt == relationalOp || ast_nt == op1 || ast_nt == op2) {
		tree_node *left_operand = get_child(astn, 0);
		ast_leaf *left_op_data = (ast_leaf *) get_data(left_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!left_op_data->is_leaf) second_pass(main_st, left_operand, curr_scope);
		/*else printf("left operand\n");*/

		ast_leaf *op_node = (ast_leaf *) get_data(get_child(astn, 1));
		/*printf("operator found here\n");*/
		
		tree_node *right_operand = get_child(astn, 2);
		ast_leaf *right_op_data = (ast_leaf *) get_data(right_operand); // blindly casting to ast_leaf
																	// thought it might be ast_node
		if (!right_op_data->is_leaf) second_pass(main_st, right_operand, curr_scope);
		/*else printf("right operand\n");*/

/*
 *        ast_node *astn_data = (ast_node *) get_data(astn);
 *
 *        printf("left: %d; right: %d\n", left_op_data->type, right_op_data->type);
 *        if (left_op_data->type == array || right_op_data->type == array) {
 *            printf("type error : array vars cannot be involved in exprs\n");
 *        }
 *        else if (ast_nt == logicalOp) {
 *            if (left_op_data->type != right_op_data->type || left_op_data->type != boolean) {
 *                printf("type error : logical operations\n");
 *                // TODO:
 *            }
 *            else {
 *                astn_data->type = boolean;
 *            }
 *        }
 *        else {
 *            if (left_op_data->type != right_op_data->type || left_op_data->type == boolean) {
 *                printf("type mismatch in expr\n");
 *                // TODO:
 *            }
 *            else if (ast_nt == relationalOp) {
 *                astn_data->type = boolean;
 *            }
 *            else {
 *                astn_data->type = left_op_data->type;
 *            }
 *        }
 */
		
	}

	if (ast_nt == declareStmt) {
/*
 *        linked_list *id_ll = ((ast_node *) get_data(get_child(astn, 0)))->ll;
 *        tree_node *dtype_node = get_child(astn, 1);
 *        id_type dtype = get_type_from_node(dtype_node);
 *
 *        int cnt = id_ll->num_nodes;
 *        for (int i = 0; i < cnt; i++) {
 *            tree_node *id_lnode = (tree_node *) ll_get(id_ll, i);
 *            char *id_var_name = ((ast_leaf *) get_data(id_lnode))->ltk->lexeme;
 *            common_id_entry *id_var_entry = find_id_for_decl(id_var_name, curr_scope);
 *            if (id_var_entry != NULL) {
 *                printf("redeclaration: %s\n", id_var_name);
 *                // TODO: redeclaration error 
 *            }
 *            else {
 *                if (dtype == array) {
 *                    tree_node *range_arrays = get_child(dtype_node, 0);
 *                    int *range_indices = get_range_from_node(range_arrays);
 *                    
 *                    arr_id_entry *entry = create_arr_entry(
 *                            id_var_name,
 *                            get_type_from_node(get_child(dtype_node, 1)),
 *                            range_indices[0],
 *                            range_indices[1],
 *                            -1, -1);
 *                    add_to_hash_map(curr_scope->arr_st, id_var_name, entry);
 *                }
 *                else {
 *                    var_id_entry *entry = create_var_entry(id_var_name, dtype, -1, -1);
 *                    add_to_hash_map(curr_scope->var_id_st, id_var_name, entry);
 *                }
 *            }
 *        }
 */
	}

	if (ast_nt == condionalStmt) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(id_node);
		tree_node *casestmt_node = get_child(astn, 2);
		tree_node *default_node = get_child(astn, 3);
		ast_leaf *default_data = (ast_leaf *) get_data(default_node);
		linked_list *casestmts_ll = ((ast_node *) get_data(casestmt_node))->ll;

		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 1));
		int start_line_num = start_data->ltk->line_num;

		// using line_num to get scope from hash map
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", start_line_num);
		scope_node *new_scope = fetch_from_hash_map(curr_scope->child_scopes, str_line_num);
		/*
		 *scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func);
		 *add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);
		 */

/*
 *        common_id_entry *entry = type_check_var(id_data, NULL, new_scope, for_use);
 *        id_type var_type = type_from_entry(entry, false);
 *        if (var_type == real || var_type == array) {
 *            printf("switch var should be int or bool\n");
 *            // TODO: type err switch var cant be these
 *        }
 *
 *        int casestmts_cnt = casestmts_ll->num_nodes;
 *        if (var_type == boolean && casestmts_cnt != 2) {
 *            printf("switch with boolean var should have exactly 2 case stmts\n");
 *            // TODO: err need exactly 2 case stmts (true/false)
 *        }
 */

		/*bool got_true = false, got_false = false;*/
		for (int i = 0; i < casestmts_ll->num_nodes; i++) {
			tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
			/*tree_node *val_node = get_child(cstmt_node, 0);*/
			tree_node *stmts_node = get_child(cstmt_node, 1);

			/*
			 *id_type val_type = get_type_from_node(val_node); // can only boolean or integer
			 *                                                // because of grammar
			 *if (val_type != var_type) {
			 *    printf("incorrect case var type\n");
			 *    // TODO: type error, should be integer
			 *}
			 *else if (val_type == boolean) {
			 *    terminal val_term = ((ast_leaf *) get_data(val_node))->label.gms.t;
			 *    got_true |= (val_term == TRUE);
			 *    got_false |= (val_term == FALSE);
			 *}
			 */

			linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
			for (int j = 0; j < stmts_ll->num_nodes; j++)
				second_pass(main_st, ll_get(stmts_ll, j), new_scope);
		}

/*
 *        if (var_type == integer) {
 *            if (default_data == NULL) {
 *                printf("default stmt required\n");
 *                // TODO: err required default stmt
 *            }
 *        }
 *        else if (var_type == boolean) {
 *            if (!got_true || !got_false) {
 *                printf("switch for bool should have both true and false cases\n");
 *                // TODO: true false both cases required
 *            }
 *
 *            if (default_data != NULL) {
 *                printf("default stmt should not be here\n");
 *                // TODO: err should not have default stmt
 *            }
 *        }
 */

		if (default_data != NULL) {
			tree_node *stmts_node = get_child(default_node, 1);

			linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
			for (int j = 0; j < stmts_ll->num_nodes; j++)
				second_pass(main_st, ll_get(stmts_ll, j), new_scope);
		}

		//
		// code gen
		//
	}

	if (ast_nt == for_loop) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = get_data(id_node);
		tree_node *range_node = get_child(astn, 1);
		tree_node *stmts_node = get_child(astn, 3);

		// insert loop var in scope
		/*
		 *var_id_entry *entry = create_var_entry(id_data->ltk->lexeme, integer, -1, -1);
		 *add_to_hash_map(new_scope->var_id_st, id_data->ltk->lexeme, entry);
		 *new_scope->loop_var_entry = entry;
		 */

		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 2));
		int start_line_num = start_data->ltk->line_num;

		// use start line num to get scope
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", start_line_num);
		scope_node *new_scope = fetch_from_hash_map(curr_scope->child_scopes, str_line_num);
		/*add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);*/

		int *range_indices = get_static_range(range_node);
		// TODO: what to do with range here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			second_pass(main_st, ll_get(stmts_ll, j), new_scope);
	}

	if (ast_nt == while_loop) {
		ast_leaf *start_data = (ast_leaf *) get_data(get_child(astn, 1));
		/*ast_leaf *end_data = (ast_leaf *) get_data(get_child(astn, 3));*/
		tree_node *aobexpr_node = get_child(astn, 0);
		tree_node *stmts_node = get_child(astn, 2);

		second_pass(main_st, aobexpr_node, curr_scope);

		/*
		 *ast_node *aobexpr_data = (ast_node *) get_data(aobexpr_node);
		 *if (aobexpr_data->type != boolean) {
		 *    printf("condition expression for while should be of type bool\n");
		 *    // TODO:
		 *}
		 */

		// using line_num to get scope
		int line_num = start_data->ltk->line_num;
		char *str_line_num = (char *) malloc(25 * sizeof(char));
		sprintf(str_line_num, "%d", line_num);
		scope_node *new_scope = (scope_node *) fetch_from_hash_map(curr_scope->child_scopes, str_line_num);
		/*
		 *scope_node *new_scope = create_new_scope(curr_scope, curr_scope->func);
		 *add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);
		 */

		// TODO: what to do with aobexpr_node here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			second_pass(main_st, ll_get(stmts_ll, j), new_scope);
	}
}

hash_map *call_semantic_analyzer (tree *ast) {
	tree_node *astn = get_root(ast);
	hash_map *main_st = create_symbol_table();
	first_pass(main_st, astn, NULL);
	second_pass(main_st, astn, NULL);
	return main_st;
}
