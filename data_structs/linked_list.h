#include <stdlib.h>
#include "../utils/gen_utils.h"

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct ____LINKED_LIST_NODE____ ll_node;

struct ____LINKED_LIST_NODE____ {
    void * data;
    ll_node * next;
    ll_node * prev;
};

typedef struct ____LINKED_LIST____ linked_list;

struct ____LINKED_LIST____ {
    ll_node * head;
    ll_node * tail;
    int num_nodes;
};

/*

destroy linked list

*/

#endif