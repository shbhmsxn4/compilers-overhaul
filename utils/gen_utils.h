/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef GEN_UTILS_H
#define GEN_UTILS_H

void assert(bool condition, char *error_string);

bool is_whitespace(char a);

char *itoa(int, char *, int);

float parse_float(char *float_str);

#endif