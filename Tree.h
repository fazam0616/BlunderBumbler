#ifndef HEADER_TREE
#define HEADER_TREE
#include <stdio.h>
#include <stdlib.h>

typedef struct Tree {
    struct Tree* left;
    struct Tree* right;
    void* data;
    int height; 
    int (*compare)(void*, void*);
} Tree;


Tree* new_tree(int (*fcn)(void*, void*), void* data);
int get_height(Tree* t);
int get_balance(Tree* t);
Tree* rotate_left(Tree* t);
Tree* rotate_right(Tree* t);
Tree* balance(Tree* t);
Tree* tree_find(Tree* t, void* data);
Tree* tree_insert(Tree* t, void* data) ;
void delete_tree(Tree* t);
Tree* tree_try(Tree* t, void* data);
#endif