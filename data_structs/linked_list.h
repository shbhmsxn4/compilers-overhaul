/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdlib.h>
#include "../utils/gen_utils.h"

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct ____LINKED_LIST_NODE____ ll_node;

struct ____LINKED_LIST_NODE____
{
    void *data;
    ll_node *next;
    ll_node *prev;
};

typedef struct ____LINKED_LIST____ linked_list;

struct ____LINKED_LIST____
{
    ll_node *head;
    ll_node *tail;
    int num_nodes;
};

linked_list *create_linked_list();

void ll_add_at(linked_list *ll, void *data, int idx);

void ll_append(linked_list *ll, void *data);

void ll_prepend(linked_list *ll, void *data);

void *ll_remove_at(linked_list *ll, int idx);

void *ll_get(linked_list *ll, int idx);

/*

destroy linked list

*/

#endif