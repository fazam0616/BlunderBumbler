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

    if (h->buckets[hash_idx] -> data != NULL)
        h->collisions++;
    
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
    h->lookups++;
    if (found) {
        h->hits++;
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
    free(H->lru);
    free(H);
}

// Compute overall hashmap health score between 0 and 1.
// Higher numbers are better (1.0 means perfectly balanced and distributed).
double hashmap_health(Hashmap* H) {
    if (!H || H->bucket_num == 0)
        return 0.0;
    
    double total_bucket_health = 0.0;
    double total_nodes = 0.0;
    double sum_squares = 0.0;
    int empty_tree = 0;
    
    for (int i = 0; i < H->bucket_num; i++) {
        int nodes = count_nodes(H->buckets[i]);
        if (nodes <= 1)
            empty_tree++;
        total_nodes += nodes;
        total_bucket_health += tree_health(H->buckets[i]);
    }

    double avg_bucket_size = (H->bucket_num > 0) ? total_nodes / H->bucket_num : 0.0;
    double variance = 0;
    for (int i = 0; i < H->bucket_num; i++) {
        variance += pow(count_nodes(H->buckets[i]) - avg_bucket_size, 2);
    }
    variance = variance / (H->bucket_num - 1);
    printf("Empty tree count:\t\t\t\t%d\n", empty_tree);
    
    double avg_bucket_health = total_bucket_health / H->bucket_num;
    printf("Average bucket health (Higher is better):\t%f\n", avg_bucket_health);
    // A lower variance means a more even distribution.
    double distribution_score = 1.0 / (1.0 + variance);  // In [0,1], with 1 for perfect distribution.
    printf("Variance (Lower is better):\t\t\t%lf\n",variance);
    // Compute load factor score:
    // Ideally, the load factor (total nodes / buckets) is low.
    // If load factor <= 1, score is 1; if higher, score decreases.
    double load_factor = (H->bucket_num > 0) ? total_nodes / H->bucket_num : 0.0;
    double load_factor_score = (load_factor <= 1.0) ? 1.0 : 1.0 / load_factor;
    printf("Load factor (Lower is better):\t\t\t%f\n", load_factor);
    // Compute hit rate score
    double hit_rate_score = 1.0;
    if (H->lookups > 0)
        hit_rate_score = (double)H->hits / H->lookups;

    printf("Hit rate (Higher is better):\t\t\t%f\n", hit_rate_score);
    
    // Combine these metrics.
    double overall = (avg_bucket_health + distribution_score + load_factor_score + hit_rate_score) / 4.0;
    return overall;
}

bool hashmap_remove(Hashmap* H, void* data){
    uint64_t bucket_idx = H->hash_fcn(data, PRIME) % H->bucket_num;
    H->buckets[bucket_idx] = tree_remove(H->buckets[bucket_idx], data);
}
