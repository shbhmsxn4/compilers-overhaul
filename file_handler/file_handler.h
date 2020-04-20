/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../utils/gen_utils.h"
#include "../data_structs/stack.h"

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

typedef struct ____FILE_HANDLER____ file_handler;

struct ____FILE_HANDLER____
{
    char *filename;
    int line_num;
    int char_num;
    int max_retract_margin;
    int num_retractable;
    FILE *fptr;
    char *buffer0;
    char *buffer1;
    bool active_buffer;
    bool file_ended;
    bool stream_ended;
    int buffer_size;
    stack *char_num_stack;
    int idx;
    bool last_populated_buffer;
};

file_handler *create_file_handler(char *filename, int buffer_size, int max_retract_margin);

// Returns EOF if stream ended, required character otherwise
char get_char(file_handler *fh);

void retract(file_handler *fh, int num_ret);

int get_line_num(file_handler *fh);

int get_char_num(file_handler *fh);

void destroy_file_handler(file_handler *fh);

#endif