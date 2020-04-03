#include <stdlib.h>
#include "../utils/gen_utils.h"

#ifndef STACK_H
#define STACK_H

typedef struct ____STACK_NODE____ stack_node;

struct ____STACK_NODE____
{
    void *data;
    stack_node *next;
};

typedef struct ____STACK____ stack;

struct ____STACK____
{
    stack_node *top;
    int num_elements;
};

// Creates a new empty stack and returns the pointer to stack
stack *create_stack();

// Pushes an element into the stack
void push(stack *s, void *data);

// Returns the topmost element of the stack without removing it
void *peek(stack *s);

// Pops and element from the stack
void *pop(stack *s);

// Returns the number of elements in the stack
int get_stack_size(stack *s);

// Destroys an empty stack
void destroy_stack(stack *s);

// Destroys a non-empty stack
void destroy_nonempty_stack(stack *s);

#endif