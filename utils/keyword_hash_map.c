#include "keyword_hash_map.h"

hash_map *create_keyword_hash_map(int num_buckets)
{
    hash_map *hm = create_hash_map(num_buckets);
    int *temp;

    temp = calloc(1, sizeof(int));
    *temp = 0;
    add_to_hash_map(hm, "declare", temp);

    temp = calloc(1, sizeof(int));
    *temp = 1;
    add_to_hash_map(hm, "module", temp);

    temp = calloc(1, sizeof(int));
    *temp = 5;
    add_to_hash_map(hm, "driver", temp);

    temp = calloc(1, sizeof(int));
    *temp = 6;
    add_to_hash_map(hm, "program", temp);

    temp = calloc(1, sizeof(int));
    *temp = 10;
    add_to_hash_map(hm, "takes", temp);

    temp = calloc(1, sizeof(int));
    *temp = 11;
    add_to_hash_map(hm, "input", temp);

    temp = calloc(1, sizeof(int));
    *temp = 14;
    add_to_hash_map(hm, "returns", temp);

    temp = calloc(1, sizeof(int));
    *temp = 17;
    add_to_hash_map(hm, "integer", temp);

    temp = calloc(1, sizeof(int));
    *temp = 18;
    add_to_hash_map(hm, "real", temp);

    temp = calloc(1, sizeof(int));
    *temp = 19;
    add_to_hash_map(hm, "boolean", temp);

    temp = calloc(1, sizeof(int));
    *temp = 20;
    add_to_hash_map(hm, "array", temp);

    temp = calloc(1, sizeof(int));
    *temp = 21;
    add_to_hash_map(hm, "of", temp);

    temp = calloc(1, sizeof(int));
    *temp = 23;
    add_to_hash_map(hm, "start", temp);

    temp = calloc(1, sizeof(int));
    *temp = 24;
    add_to_hash_map(hm, "end", temp);

    temp = calloc(1, sizeof(int));
    *temp = 27;
    add_to_hash_map(hm, "get_value", temp);

    temp = calloc(1, sizeof(int));
    *temp = 28;
    add_to_hash_map(hm, "print", temp);

    temp = calloc(1, sizeof(int));
    *temp = 29;
    add_to_hash_map(hm, "true", temp);

    temp = calloc(1, sizeof(int));
    *temp = 30;
    add_to_hash_map(hm, "false", temp);

    temp = calloc(1, sizeof(int));
    *temp = 34;
    add_to_hash_map(hm, "use", temp);

    temp = calloc(1, sizeof(int));
    *temp = 35;
    add_to_hash_map(hm, "with", temp);

    temp = calloc(1, sizeof(int));
    *temp = 36;
    add_to_hash_map(hm, "parameters", temp);

    temp = calloc(1, sizeof(int));
    *temp = 41;
    add_to_hash_map(hm, "AND", temp);

    temp = calloc(1, sizeof(int));
    *temp = 42;
    add_to_hash_map(hm, "OR", temp);

    temp = calloc(1, sizeof(int));
    *temp = 49;
    add_to_hash_map(hm, "switch", temp);

    temp = calloc(1, sizeof(int));
    *temp = 50;
    add_to_hash_map(hm, "case", temp);

    temp = calloc(1, sizeof(int));
    *temp = 51;
    add_to_hash_map(hm, "break", temp);

    temp = calloc(1, sizeof(int));
    *temp = 52;
    add_to_hash_map(hm, "default", temp);

    temp = calloc(1, sizeof(int));
    *temp = 53;
    add_to_hash_map(hm, "for", temp);

    temp = calloc(1, sizeof(int));
    *temp = 54;
    add_to_hash_map(hm, "in", temp);

    temp = calloc(1, sizeof(int));
    *temp = 55;
    add_to_hash_map(hm, "while", temp);

    temp = NULL;
    return hm;
}