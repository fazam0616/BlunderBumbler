#ifndef HEADER_HASHMAP
#define HEADER_HASHMAP
#include <stdint.h>
#include "Tree.h"
// #include "List.h"

#define PRIME 1007557

struct LRUList;

typedef struct Hashmap{
    uint64_t (*hash_fcn) (void *, unsigned long);
    Tree** buckets;
    unsigned long size;
    int bucket_num;
    struct LRUList* lru;
} Hashmap;

Hashmap* new_hashmap(uint64_t (*fcnA) (void *, unsigned long), int (*fcnB)(void*,void*), int b_num);
Tree* hash_insert(Hashmap* h, void* data);
void * hash_find(Hashmap* h, void* data);
void delete_hashmap(Hashmap* H);
void * hash_try(Hashmap* h, void* data);


#endif