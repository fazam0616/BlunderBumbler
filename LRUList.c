#include "LRUList.h"

LRUList* new_lru_list() {
    LRUList* list = malloc(sizeof(LRUList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void lru_move_to_front(LRUList* list, GameBranch* branch) {
    if (branch == list->head) return;  // Already at front

    // Remove from current position
    if (branch->lru_prev) branch->lru_prev->lru_next = branch->lru_next;
    if (branch->lru_next) branch->lru_next->lru_prev = branch->lru_prev;
    if (branch == list->tail) list->tail = branch->lru_prev;

    // Insert at front
    branch->lru_next = list->head;
    branch->lru_prev = NULL;

    if (list->head) list->head->lru_prev = branch;
    list->head = branch;

    if (list->tail == NULL) list->tail = branch;  // First item in list
}

void lru_insert(LRUList* list, GameBranch* node) {
    node->lru_next = list->head;
    node->lru_prev = NULL;
    
    if (list->head) list->head->lru_prev = node;
    list->head = node;

    if (!list->tail) list->tail = node;  // First node case

    list->size++;
}



GameBranch* lru_evict(LRUList* list) {
    if (list->tail == NULL) return NULL;  // Nothing to evict

    GameBranch* to_remove = list->tail;
    list->tail = to_remove->lru_prev;
    if (list->tail) list->tail->lru_next = NULL;
    else list->head = NULL;  // List is now empty

    list->size--;
    return to_remove;
}

bool pruneCheck(GameBranch* prev, GameBranch* next, Hashmap* SearchTree){
    int s_i, s_j, d_i, d_j;
    Piece type;
    GameBranch* to_evict;
    int num = 0;
    int skip = 0;
    int target = SearchTree->size * 0.1;
    //Was board significantly altered? (Piece captured, pawn moved)
    if ((prev && next)){
        type = find_move(&prev->board, &next->board, &s_i, &s_j, &d_i, &d_j);
        if (type >= 0 || getBitBoardBit(&prev->board.bitboards[PAWN], s_i, s_j)){
            to_evict = lru_evict(SearchTree->lru);
            while (num < target && skip < target){
                if (to_evict->depth < prev->depth){
                    //EVICTING
                    GameBranch* parent = to_evict->prev_branch;
                    if (parent){
                        printf("Evicted %d branches, skipped %d branches\n", num, skip);
                        delete_tree(parent->possible_branches);
                        parent->possible_branches = NULL;
                        if (parent->best == to_evict)
                            parent->best = NULL;
                        
                        hashmap_remove(SearchTree, to_evict);
                        
                    }
                    deleteGameBranch(to_evict);
                    num++;
                }
                else {
                    //DONT EVICT
                    lru_move_to_front(SearchTree->lru, to_evict);
                    skip++;
                }

                to_evict = lru_evict(SearchTree->lru);
            }
            printf("Evicted %d branches, skipped over %d branches.\n\n", num, skip);
        }
    }
}