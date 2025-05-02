    #ifndef HEADER_HASHMAP
    #define HEADER_HASHMAP
    #include <stdint.h>
    #include <stdlib.h>
    #include "Tree.h"
    #include <stdbool.h>

    #define PRIME 1007557

    struct LRUList;

    typedef struct Hashmap{
        uint64_t (*hash_fcn) (void *, unsigned long);
        Tree** buckets;
        unsigned long size;
        int bucket_num;
        struct LRUList* lru;

        // New fields to track performance:
        unsigned long lookups;     // Total number of lookup attempts
        unsigned long hits;        // Total number of successful lookups
        unsigned long collisions;
    } Hashmap;

    Hashmap* new_hashmap(uint64_t (*fcnA) (void *, unsigned long), int (*fcnB)(void*,void*), int b_num);
    Tree* hash_insert(Hashmap* h, void* data);
    void * hash_find(Hashmap* h, void* data);
    void delete_hashmap(Hashmap* H);
    void * hash_try(Hashmap* h, void* data);
    double hashmap_health(Hashmap* H);
    bool hashmap_remove(Hashmap* H, void* data);


    #endif