/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "terminal_hash_map.h"

hash_map *create_terminal_hash_map(int num_buckets)
{
    hash_map *hm = create_hash_map(num_buckets);
    int *temp;

    temp = calloc(1, sizeof(int));
    *temp = 0;
    add_to_hash_map(hm, "DECLARE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 1;
    add_to_hash_map(hm, "MODULE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 2;
    add_to_hash_map(hm, "ID", temp);

    temp = calloc(1, sizeof(int));
    *temp = 3;
    add_to_hash_map(hm, "SEMICOL", temp);

    temp = calloc(1, sizeof(int));
    *temp = 4;
    add_to_hash_map(hm, "DRIVERDEF", temp);

    temp = calloc(1, sizeof(int));
    *temp = 5;
    add_to_hash_map(hm, "DRIVER", temp);

    temp = calloc(1, sizeof(int));
    *temp = 6;
    add_to_hash_map(hm, "PROGRAM", temp);

    temp = calloc(1, sizeof(int));
    *temp = 7;
    add_to_hash_map(hm, "DRIVERENDDEF", temp);

    temp = calloc(1, sizeof(int));
    *temp = 8;
    add_to_hash_map(hm, "DEF", temp);

    temp = calloc(1, sizeof(int));
    *temp = 9;
    add_to_hash_map(hm, "ENDDEF", temp);

    temp = calloc(1, sizeof(int));
    *temp = 10;
    add_to_hash_map(hm, "TAKES", temp);

    temp = calloc(1, sizeof(int));
    *temp = 11;
    add_to_hash_map(hm, "INPUT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 12;
    add_to_hash_map(hm, "SQBO", temp);

    temp = calloc(1, sizeof(int));
    *temp = 13;
    add_to_hash_map(hm, "SQBC", temp);

    temp = calloc(1, sizeof(int));
    *temp = 14;
    add_to_hash_map(hm, "RETURNS", temp);

    temp = calloc(1, sizeof(int));
    *temp = 15;
    add_to_hash_map(hm, "COLON", temp);

    temp = calloc(1, sizeof(int));
    *temp = 16;
    add_to_hash_map(hm, "COMMA", temp);

    temp = calloc(1, sizeof(int));
    *temp = 17;
    add_to_hash_map(hm, "INTEGER", temp);

    temp = calloc(1, sizeof(int));
    *temp = 18;
    add_to_hash_map(hm, "REAL", temp);

    temp = calloc(1, sizeof(int));
    *temp = 19;
    add_to_hash_map(hm, "BOOLEAN", temp);

    temp = calloc(1, sizeof(int));
    *temp = 20;
    add_to_hash_map(hm, "ARRAY", temp);

    temp = calloc(1, sizeof(int));
    *temp = 21;
    add_to_hash_map(hm, "OF", temp);

    temp = calloc(1, sizeof(int));
    *temp = 22;
    add_to_hash_map(hm, "RANGEOP", temp);

    temp = calloc(1, sizeof(int));
    *temp = 23;
    add_to_hash_map(hm, "START", temp);

    temp = calloc(1, sizeof(int));
    *temp = 24;
    add_to_hash_map(hm, "END", temp);

    temp = calloc(1, sizeof(int));
    *temp = 25;
    add_to_hash_map(hm, "BO", temp);

    temp = calloc(1, sizeof(int));
    *temp = 26;
    add_to_hash_map(hm, "BC", temp);

    temp = calloc(1, sizeof(int));
    *temp = 27;
    add_to_hash_map(hm, "GET_VALUE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 28;
    add_to_hash_map(hm, "PRINT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 29;
    add_to_hash_map(hm, "TRUE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 30;
    add_to_hash_map(hm, "FALSE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 31;
    add_to_hash_map(hm, "NUM", temp);

    temp = calloc(1, sizeof(int));
    *temp = 32;
    add_to_hash_map(hm, "RNUM", temp);

    temp = calloc(1, sizeof(int));
    *temp = 33;
    add_to_hash_map(hm, "ASSIGNOP", temp);

    temp = calloc(1, sizeof(int));
    *temp = 34;
    add_to_hash_map(hm, "USE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 35;
    add_to_hash_map(hm, "WITH", temp);

    temp = calloc(1, sizeof(int));
    *temp = 36;
    add_to_hash_map(hm, "PARAMETERS", temp);

    temp = calloc(1, sizeof(int));
    *temp = 37;
    add_to_hash_map(hm, "PLUS", temp);

    temp = calloc(1, sizeof(int));
    *temp = 38;
    add_to_hash_map(hm, "MINUS", temp);

    temp = calloc(1, sizeof(int));
    *temp = 39;
    add_to_hash_map(hm, "MUL", temp);

    temp = calloc(1, sizeof(int));
    *temp = 40;
    add_to_hash_map(hm, "DIV", temp);

    temp = calloc(1, sizeof(int));
    *temp = 41;
    add_to_hash_map(hm, "AND", temp);

    temp = calloc(1, sizeof(int));
    *temp = 42;
    add_to_hash_map(hm, "OR", temp);

    temp = calloc(1, sizeof(int));
    *temp = 43;
    add_to_hash_map(hm, "LT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 44;
    add_to_hash_map(hm, "LE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 45;
    add_to_hash_map(hm, "GT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 46;
    add_to_hash_map(hm, "GE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 47;
    add_to_hash_map(hm, "EQ", temp);

    temp = calloc(1, sizeof(int));
    *temp = 48;
    add_to_hash_map(hm, "NE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 49;
    add_to_hash_map(hm, "SWITCH", temp);

    temp = calloc(1, sizeof(int));
    *temp = 50;
    add_to_hash_map(hm, "CASE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 51;
    add_to_hash_map(hm, "BREAK", temp);

    temp = calloc(1, sizeof(int));
    *temp = 52;
    add_to_hash_map(hm, "DEFAULT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 53;
    add_to_hash_map(hm, "FOR", temp);

    temp = calloc(1, sizeof(int));
    *temp = 54;
    add_to_hash_map(hm, "IN", temp);

    temp = calloc(1, sizeof(int));
    *temp = 55;
    add_to_hash_map(hm, "WHILE", temp);

    temp = calloc(1, sizeof(int));
    *temp = 56;
    add_to_hash_map(hm, "EPS", temp);

    temp = calloc(1, sizeof(int));
    *temp = 57;
    add_to_hash_map(hm, "DOLLAR", temp);

    temp = NULL;
    return hm;
}