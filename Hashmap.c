#include "Hashmap.h"
#include "LRUList.h"

Hashmap* new_hashmap(uint64_t (*fcnA) (void *, unsigned long), int (*fcnB)(void*,void*), int b_num){
    Hashmap* H = malloc( sizeof(Hashmap));
    if (H== NULL)
        return NULL;

    H->buckets = malloc(b_num* sizeof(Tree*));
    if (H->buckets == NULL){
        printf("Failed to allocate buckets to hashmap\n");
        free(H);
        return NULL;
    }


    for (int i = 0; i < b_num; i++){
        H->buckets[i] = new_tree(fcnB, NULL);
        if (H->buckets[i] == NULL){
            printf("Failed to allocate trees for hashmap\n");
            delete_hashmap(H);
            return NULL;
        }
    }
    H->lru = new_lru_list();
    H->bucket_num = b_num;
    H->hash_fcn = fcnA;
    H->size = 0;
    return H;    
}

Tree* hash_insert(Hashmap* h, void* data) {
    h->size++;
    uint64_t hash = h->hash_fcn(data, PRIME);
    int hash_idx = hash % h->bucket_num;
    
    Tree* inserted = tree_insert(h->buckets[hash_idx], data);

    lru_insert(h->lru, data);  // Add to LRU list
    return inserted;
}


void * hash_try(Hashmap* h, void* data){
    uint64_t hash = h->hash_fcn(data, PRIME);
    int hash_idx = hash % h->bucket_num;
    return tree_try(h->buckets[hash_idx], data)->data;
}

void * hash_find(Hashmap* h, void* data){
    uint64_t hash = h->hash_fcn(data, PRIME);
    int hash_idx = hash % h->bucket_num;
    Tree* found = tree_find(h->buckets[hash_idx], data);
    if (found) {
        lru_move_to_front(h->lru, (GameBranch*)found->data);
        return found->data;
    }
    return NULL;
}

void delete_hashmap(Hashmap* H){
    for (int i = 0; i < H->bucket_num; i++){
        delete_tree(H->buckets[i]);
    }
    free(H->buckets);
    free(H);
}