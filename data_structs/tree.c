#include "tree.h"

tree *create_tree()
{
    tree *new_tree = (tree *)calloc(1, sizeof(tree));
    new_tree->root = NULL;
    return new_tree;
}

void set_root(tree *t, void *data)
{
    assert(t->root == NULL, "setting root of empty tree");

    tree_node *root_node = (tree_node *)calloc(1, sizeof(tree_node));

    root_node->data = data;
    root_node->num_children = 0;
    root_node->max_children = 1;
    root_node->parent = NULL;
    root_node->child = (tree_node **)calloc(1, sizeof(tree_node *));

    t->root = root_node;
}

tree_node *get_root(tree *t)
{
    return t->root;
}

void expand_child_array(tree_node *n)
{
    assert(n->num_children == n->max_children, "tree node child array expansion function called on overflow");

    n->child = (tree_node **)realloc((void *)(n->child), 2 * (n->max_children) * sizeof(tree_node *));

    assert(n->child != NULL, "expansion of tree node child array successful");

    n->max_children = 2 * n->max_children;
}

void add_child_at(tree_node *n, void *data, int idx)
{
    assert(idx <= n->num_children && idx >= 0, "tree add child called with valid index");

    tree_node *new_node = (tree_node *)calloc(1, sizeof(tree_node));

    new_node->parent = n;
    if (data == NULL)
    {
        new_node->data = NULL;
    }
    else
    {
        new_node->data = data;
    }
    new_node->num_children = 0;
    new_node->max_children = 1;
    new_node->child = (tree_node **)calloc(1, sizeof(tree_node *));

    assert(n->num_children >= 0 && n->num_children <= n->max_children, "node passed to tree add child is valid");

    if (n->num_children == n->max_children)
    {
        expand_child_array(n);
    }

    for (int i = n->num_children; i > idx; i--)
    {
        n->child[i] = n->child[i - 1];
    }

    n->child[idx] = new_node;

    (n->num_children)++;
}

void add_child(tree_node *n, void *data)
{
    add_child_at(n, data, n->num_children);
}

int get_num_children(tree_node *n)
{
    return n->num_children;
}

void *get_data(tree_node *n)
{
    return n->data;
}

tree_node *get_child(tree_node *n, int idx)
{
    assert(idx < n->num_children, "requested child is valid");
    return n->child[idx];
}

tree_node *get_parent(tree_node *n)
{
    return n->parent;
}

void *remove_child(tree_node *n, int idx)
{
    assert(idx < n->num_children, "removal of valid child requested");
    assert(n->child[idx]->num_children == 0, "removal of leaf node requested");

    tree_node *old_node = n->child[idx];

    for (int i = idx; i < (n->num_children - 1); i++)
    {
        n->child[i] = n->child[i + 1];
    }

    n->num_children = n->num_children - 1;
    void *old_data = old_node->data;
    free(old_node->child);
    old_node->parent = NULL;
    old_node->data = NULL;
    free(old_node);
    return old_data;
}

void set_data(tree_node *n, void *data)
{
    assert(n->data == NULL, "setting tree data on empty node");
    n->data = data;
}

tree_node *create_tree_node(void *data)
{
    tree_node *new_node = (tree_node *)calloc(1, sizeof(tree_node));
    new_node->parent = NULL;
    new_node->data = data;
    new_node->num_children = 0;
    new_node->max_children = 1;
    new_node->child = (tree_node **)calloc(1, sizeof(tree_node *));
    return new_node;
}