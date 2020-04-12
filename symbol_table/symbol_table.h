#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../data_structs/tree.h"
#include "./symbol_table_def.h"
#include "../ast/generate_ast.h"

hash_map *create_symbol_table ();
id_type terminal_to_type (terminal t);
id_type get_type_from_node (tree_node *node);
int get_width_from_type (id_type t); 
int* get_range_from_node (tree_node *node);
scope_node *create_new_scope (scope_node *parent, func_entry *func);
var_id_entry *create_var_entry (char *lexeme, id_type type, int width, int offset);
arr_id_entry *create_arr_entry (char *lexeme, id_type type, int rstart, int rend, int width, int offset);
func_entry *create_func_entry (char *name, bool is_declared, bool is_defined, bool is_called, int offset, int width);
common_id_entry *find_id (char *lexeme, scope_node *curr_scope, bool is_recursive);
common_id_entry *find_id_rec (char *lexeme, scope_node *curr_scope);
common_id_entry *find_id_in_scope (char *lexeme, scope_node *curr_scope);
param_node *find_id_in_paramsll (char *lexeme, linked_list *pll);
param_node *find_id_in_inputparams (char *lexeme, func_entry *entry);
param_node *find_id_in_outputparams (char *lexeme, func_entry *entry);
common_id_entry *param_to_st_entry (param_node *p);
common_id_entry *find_id_for_decl (char *lexeme, scope_node *curr_scope);
common_id_entry *find_id_for_use (char *lexeme, scope_node *curr_scope);
common_id_entry *find_id_for_assign (char *lexeme, scope_node *curr_scope);
common_id_entry *find_id_for (char *lexeme, scope_node *curr_scope, reason_flag need_for);
bool match_array_type (arr_id_entry *arr1, arr_id_entry *arr2);
int bound_type_check (arr_id_entry *entry, ast_leaf *index_data, scope_node *curr_scope);
common_id_entry *type_check_var (ast_leaf *id_data, ast_leaf *index_node, scope_node *curr_scope, reason_flag need_for);
id_type type_from_entry (common_id_entry *entry, bool indexed);
bool is_same_type (common_id_entry *a, common_id_entry *b);
void arr_assign_offset (arr_id_entry *entry, func_entry *func);
void var_assign_offset (var_id_entry *entry, func_entry *func);

#endif
