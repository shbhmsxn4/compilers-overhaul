#include "../data_structs/tree.h"
#include "../data_structs/linked_list.h"
#include "../lang_specs/entities.h"
#include "../data_structs/grammar.h"

#ifndef GENERATE_AST_H
#define GENERATE_AST_H

typedef struct ____AST_NODE____ ast_node;

struct ____AST_NODE____
{
    gm_unit *label;
    linked_list *ll;
};

typedef struct ____AST_LEAF____ ast_leaf;

struct ____AST_LEAF____
{
    gm_unit *label;
    lexical_token *ltk;
};

tree *generate_ast(tree *pt)
{
}

#endif