#ifndef HEADER_LIST
#define HEADER_LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define GROWTH 1.5

typedef struct List{
    void** data;
    unsigned long size;
    unsigned long capacity;
} List;

bool append(List* l, void* address);
void* get(List* l, unsigned int idx);
List* new_list();
void delete_list(List* l);
bool is_empty(List* L);

#endif