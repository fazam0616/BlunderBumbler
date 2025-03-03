#ifndef HEADER_LRULIST
#define HEADER_LRULIST
#include "Chess.h"

typedef struct LRUList {
    GameBranch* head;
    GameBranch* tail;
    unsigned long size;
} LRUList;

LRUList* new_lru_list();
void lru_move_to_front(LRUList* list, GameBranch* branch);
GameBranch* lru_evict(LRUList* list);
void lru_insert(LRUList* list, GameBranch* node);
bool pruneCheck(GameBranch* prev, GameBranch* next, Hashmap* SearchTree);

#endif