/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "gen_utils.h"

void assert(bool condition, char *error_string)
{
    if (!condition)
    {
        printf("ERROR: Assertion failed - %s\n", error_string);
        exit(1);
    }
}

bool is_whitespace(char a)
{
    if (a == '\n')
        return true;
    if (a == '\r')
        return true;
    if (a == '\t')
        return true;
    if (a == ' ')
        return true;
    return false;
}

char *itoa(int value, char *str, int base)
{
    sprintf(str, "%d", value);
    return str;
}