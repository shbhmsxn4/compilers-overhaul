/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include <stdlib.h>
#include <string.h>
#include "../utils/gen_utils.h"

#ifndef HASH_MAP_H
#define HASH_MAP_H

typedef struct ____HASH_MAP_NODE____ hm_node;

struct ____HASH_MAP_NODE____
{
    char *string;
    void *data;
    hm_node *next;
};

struct ____HASH_MAP_BUCKET____
{
    hm_node *first;
    int num_nodes;
};

typedef struct ____HASH_MAP_BUCKET____ hm_bucket;

struct ____HASH_MAP____
{
    hm_bucket **buckets;
    int num_buckets;
};

typedef struct ____HASH_MAP____ hash_map;

hash_map *create_hash_map(int num_buckets);

void add_to_hash_map(hash_map *hm, char *string, void *data);

void *fetch_from_hash_map(hash_map *hm, char *string);

void *remove_from_hash_map(hash_map *hm, char *string);

void destroy_hash_map(hash_map *hm);

hm_node *get_all_hm_nodes (hash_map *map);
// will map strings to void *

/*

functions to be implemented:

destroy hash map

*/

#endif
