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