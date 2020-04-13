#include "hash_map.h"

int hash(char *string)
{
    int i = 0;
    int hash = 0;
    while (string[i] != '\0')
    {
        hash += (int)string[i];
        i++;
    }
    return hash;
}

hash_map *create_hash_map(int num_buckets)
{

    hash_map *new_hm = (hash_map *)calloc(1, sizeof(hash_map));
    new_hm->num_buckets = num_buckets;
    new_hm->buckets = (hm_bucket **)calloc(num_buckets, sizeof(hm_bucket *));
    assert(new_hm->buckets != NULL, "hash map bucket space allocated");
    for (int i = 0; i < num_buckets; i++)
    {
        new_hm->buckets[i] = (hm_bucket *)calloc(1, sizeof(hm_bucket));
        (new_hm->buckets[i])->first = NULL;
        (new_hm->buckets[i])->num_nodes = 0;
    }

    return new_hm;
}

void add_to_bucket(hm_bucket *b, char *string, void *data)
{
    hm_node *new_hm_node = (hm_node *)calloc(1, sizeof(hm_node));
    new_hm_node->string = string;
    new_hm_node->data = data;
    new_hm_node->next = NULL;

    new_hm_node->next = b->first;
    b->first = new_hm_node;
    b->num_nodes = b->num_nodes + 1;
}

void add_to_hash_map(hash_map *hm, char *string, void *data)
{
    assert(fetch_from_hash_map(hm, string) == NULL, "entry being added doesn't already exist in hash map");

    int bucket_idx = hash(string) % (hm->num_buckets);
    add_to_bucket(hm->buckets[bucket_idx], string, data);
}

void *fetch_from_bucket(hm_bucket *b, char *string)
{
    if (b->num_nodes == 0)
    {
        return NULL;
    }
    else
    {
        hm_node *temp = b->first;
        void *data = NULL;
        while (temp != NULL)
        {
            if (strcmp(string, temp->string) == 0)
            {
                data = temp->data;
                break;
            }
            temp = temp->next;
        }
        return data;
    }
}

void *fetch_from_hash_map(hash_map *hm, char *string)
{
    int bucket_idx = hash(string) % (hm->num_buckets);
    return fetch_from_bucket(hm->buckets[bucket_idx], string);
}

void *remove_from_bucket(hm_bucket *b, char *string)
{
    hm_node *temp = b->first;
    if (strcmp(temp->string, string) == 0)
    {
        b->first = temp->next;
        b->num_nodes = b->num_nodes - 1;
        return temp->data;
    }
    else
    {
        temp = temp->next;
        hm_node *prev = b->first;
        while (strcmp(temp->string, string) != 0)
        {
            temp = temp->next;
            prev = prev->next;
        }
        prev->next = temp->next;
        b->num_nodes = b->num_nodes - 1;
        return temp->data;
    }
}

void *remove_from_hash_map(hash_map *hm, char *string)
{
    assert(fetch_from_hash_map(hm, string) != NULL, "entry to be deleted exists in hash map");

    int bucket_idx = hash(string) % (hm->num_buckets);

    return remove_from_bucket(hm->buckets[bucket_idx], string);
}

void destroy_hm_node(hm_node *n)
{
    if (n->next != NULL)
    {
        destroy_hm_node(n->next);
    }
    free(n);
}

void destroy_hm_bucket(hm_bucket *b)
{
    if (b->num_nodes != 0)
    {
        destroy_hm_node(b->first);
    }
    free(b);
}

void destroy_hash_map(hash_map *hm)
{
    for (int i = 0; i < hm->num_buckets; i++)
    {
        destroy_hm_bucket(hm->buckets[i]);
    }
    free(hm->buckets);
    free(hm);
}

hm_node *get_all_hm_nodes (hash_map *map) {
	hm_node *tmp = (hm_node *) malloc(sizeof(hm_node));
	tmp->next = NULL;

	hm_node *curr_last_ptr = tmp;
	int num_buckets = map->num_buckets;
	for (int i = 0; i < num_buckets; i++) {
		hm_node *head = map->buckets[i]->first;
		if (head != NULL) {
			curr_last_ptr->next = head;

			while (curr_last_ptr->next != NULL) {
				curr_last_ptr = curr_last_ptr->next;
			}
		}
	}

	hm_node *ret = tmp->next;
	free(tmp);
	return ret;
}
