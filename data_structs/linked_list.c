/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "linked_list.h"

linked_list *create_linked_list()
{
    linked_list *ll = calloc(1, sizeof(linked_list));
    ll->num_nodes = 0;
    ll->head = NULL;
    ll->tail = NULL;
    return ll;
}

void ll_add_at(linked_list *ll, void *data, int idx)
{
    assert(idx <= ll->num_nodes && idx >= 0, "linked list insertion index valid");

    ll_node *new_node = calloc(1, sizeof(ll_node));
    new_node->data = data;

    if (ll->num_nodes == 0)
    {
        new_node->next = NULL;
        new_node->prev = NULL;
        ll->head = new_node;
        ll->tail = new_node;
        ll->num_nodes = 1;
    }
    else
    {
        if (idx == 0)
        {
            new_node->next = ll->head;
            ll->head->prev = new_node;
            new_node->prev = NULL;
            ll->head = new_node;
            (ll->num_nodes)++;
        }
        else if (idx == ll->num_nodes)
        {
            ll->tail->next = new_node;
            new_node->prev = ll->tail;
            new_node->next = NULL;
            ll->tail = new_node;
            (ll->num_nodes)++;
        }
        else
        {
            ll_node *temp;
            ll_node *pre_temp;
            pre_temp = ll->head;
            temp = pre_temp->next;
            int i = 1;
            while (i < idx)
            {
                temp = temp->next;
                pre_temp = pre_temp->next;
                i++;
            }
            pre_temp->next = new_node;
            temp->prev = new_node;
            new_node->next = temp;
            new_node->prev = pre_temp;
            (ll->num_nodes)++;
        }
    }
}

void ll_append(linked_list *ll, void *data)
{
    ll_add_at(ll, data, ll->num_nodes);
}

void ll_prepend(linked_list *ll, void *data)
{
    ll_add_at(ll, data, 0);
}

void *ll_remove_at(linked_list *ll, int idx)
{
    assert(idx < ll->num_nodes && idx >= 0, "index valid while removing from linked list");

    if (ll->num_nodes == 1)
    {
        ll_node *old_node = ll->head;
        ll->head = NULL;
        ll->tail = NULL;
        ll->num_nodes = 0;
        void *data = old_node->data;
        old_node->data = NULL;
        old_node->next = NULL;
        old_node->prev = NULL;
        free(old_node);
        return data;
    }
    else
    {
        if (idx == 0)
        {
            ll_node *old_node = ll->head;
            ll->head = old_node->next;
            ll->head->prev = NULL;
            (ll->num_nodes)--;
            void *data = old_node->data;
            old_node->data = NULL;
            old_node->next = NULL;
            old_node->prev = NULL;
            free(old_node);
            return data;
        }
        else if (idx == ll->num_nodes - 1)
        {
            ll_node *old_node = ll->tail;
            ll->tail = old_node->prev;
            ll->tail->next = NULL;
            (ll->num_nodes)--;
            void *data = old_node->data;
            old_node->data = NULL;
            old_node->next = NULL;
            old_node->prev = NULL;
            free(old_node);
            return data;
        }
        else
        {
            ll_node *succ;
            ll_node *pred;
            ll_node *old_node;
            pred = ll->head;
            old_node = pred->next;
            succ = old_node->next;
            int i = 1;
            while (i < idx)
            {
                pred = pred->next;
                old_node = old_node->next;
                succ = succ->next;
                i++;
            }
            pred->next = succ;
            succ->prev = pred;
            void *data = old_node->data;
            old_node->data = NULL;
            old_node->next = NULL;
            old_node->prev = NULL;
            free(old_node);
            return data;
        }
    }
}

void *ll_get(linked_list *ll, int idx)
{
    assert(idx < ll->num_nodes && idx >= 0, "index valid while getting from linked list");

    if (idx == 0)
    {
        return ll->head->data;
    }
    else if (idx == ll->num_nodes - 1)
    {
        return ll->tail->data;
    }
    else
    {
        int i = 0;
        ll_node *temp = ll->head;
        while (i < idx)
        {
            temp = temp->next;
            i++;
        }
        return temp->data;
    }
}