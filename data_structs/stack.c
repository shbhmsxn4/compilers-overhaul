#include "stack.h"

stack *create_stack()
{
    stack *new_stack = (stack *)calloc(1, sizeof(stack));
    new_stack->top = NULL;
    new_stack->num_elements = 0;
    return new_stack;
}

void push(stack *s, void *data)
{
    stack_node *new_node = (stack_node *)calloc(1, sizeof(stack_node));
    new_node->data = data;
    new_node->next = s->top;
    s->top = new_node;
    s->num_elements = s->num_elements + 1;
}

void *peek(stack *s)
{
    if (s->top == NULL)
    {
        return NULL;
    }
    else
    {
        return s->top->data;
    }
}

void *pop(stack *s)
{
    stack_node *old_node = s->top;
    if (old_node == NULL)
    {
        return NULL;
    }
    else
    {
        s->top = old_node->next;
        s->num_elements = s->num_elements - 1;
        old_node->next = NULL;
        void *data = old_node->data;
        old_node->data = NULL;
        free(old_node);
        return data;
    }
}

int get_stack_size(stack *s)
{
    return s->num_elements;
}

void destroy_stack(stack *s)
{
    assert(s->top == NULL && s->num_elements == 0, "destroy stack called with an empty stack");
    free(s);
}

void destroy_nonempty_stack(stack *s)
{
    while (pop(s) != NULL)
        ;
    free(s);
}