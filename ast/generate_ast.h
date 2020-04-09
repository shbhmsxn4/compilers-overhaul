#include "../data_structs/tree.h"
#include "../data_structs/linked_list.h"
#include "../lang_specs/entities.h"
#include "../data_structs/grammar.h"
#include "../utils/nonterminal_name.h"

void print_ast_subtree(tree_node *, int);

#ifndef GENERATE_AST_H
#define GENERATE_AST_H

typedef struct ____AST_NODE____ ast_node;

struct ____AST_NODE____
{
    bool is_leaf;
    gm_unit label;
    linked_list *ll;
};

typedef struct ____AST_LEAF____ ast_leaf;

struct ____AST_LEAF____
{
    bool is_leaf;
    gm_unit label;
    lexical_token *ltk;
};

tree *generate_ast(tree *pt);

#endif