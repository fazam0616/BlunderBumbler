#include "List.h"

bool append(List* l, void* address){
    if (l->size+1 >= l->capacity){
        void** new_data = malloc(l->capacity * GROWTH * sizeof(void*));
        if (new_data == NULL){
            printf("Insufficient memory, unable to grow list\n");
            return false;
        }
        
        memcpy(new_data, l->data, l->size * sizeof(void*));
        l->capacity = l->capacity * GROWTH;
        free(l->data);
        l->data = new_data;
    }

    l->data[l->size++] = address;

    return true;
}

bool is_empty(List* l){
    return l->size == 0;
}

void* get(List* l, unsigned int idx){
    if (idx >= l->size){
        return NULL;
    }

    return l->data[idx];
}

List* new_list(){
    List* l = malloc(sizeof(List));
    if (l != NULL){
        l->capacity = 4;
        l->size = 0;
        l->data = malloc(l->capacity* sizeof(void*));
        if (!l->data){
            printf("Failed to allocate list buffer\n");
            free(l);
            return NULL;
        }
    }
    else{
        printf("Failed to allocate list\n");
    }

    return l;
}

void delete_list(List* l){
    if (l){
        free(l->data);
        free(l);
    }
}