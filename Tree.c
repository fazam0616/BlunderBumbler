#include "Tree.h"
#include <stdlib.h>

Tree* new_tree(int (*fcn)(void*, void*), void* data) {
    Tree* t = malloc(sizeof(Tree));
    if (t == NULL){
        printf("Failed to allocate tree\n");
        return NULL;
    } 

    t->data = data;
    t->compare = fcn;
    t->left = NULL;
    t->right = NULL;
    t->height = 1;  // Initial height is 1
    return t;
}


int get_height(Tree* t) {
    return (t == NULL) ? 0 : t->height;
}

int get_balance(Tree* t) {
    return (t == NULL) ? 0 : get_height(t->left) - get_height(t->right);
}

Tree* rotate_left(Tree* t) {
    Tree* new_root = t->right;
    t->right = new_root->left;
    new_root->left = t;

    // Update heights
    t->height = 1 + (get_height(t->left) > get_height(t->right) ? get_height(t->left) : get_height(t->right));
    new_root->height = 1 + (get_height(new_root->left) > get_height(new_root->right) ? get_height(new_root->left) : get_height(new_root->right));

    return new_root;
}

Tree* rotate_right(Tree* t) {
    Tree* new_root = t->left;
    t->left = new_root->right;
    new_root->right = t;

    // Update heights
    t->height = 1 + (get_height(t->left) > get_height(t->right) ? get_height(t->left) : get_height(t->right));
    new_root->height = 1 + (get_height(new_root->left) > get_height(new_root->right) ? get_height(new_root->left) : get_height(new_root->right));

    return new_root;
}

Tree* balance(Tree* t) {
    int balance_factor = get_balance(t);

    // Left-heavy case
    if (balance_factor > 1) {
        if (get_balance(t->left) < 0) {
            t->left = rotate_left(t->left);  // Left-Right case
        }
        return rotate_right(t);  // Left-Left case
    }

    // Right-heavy case
    if (balance_factor < -1) {
        if (get_balance(t->right) > 0) {
            t->right = rotate_right(t->right);  // Right-Left case
        }
        return rotate_left(t);  // Right-Right case
    }

    // Already balanced
    return t;
}


Tree* tree_find(Tree* t, void* data){
    if (t == NULL || t->data == NULL)
        return NULL;

    int comp = t->compare(t->data, data);
    if (comp == 0)
        return t;
    else if (comp > 0)
        return tree_find(t->right, data);
    else    
        return tree_find(t->left, data);
}

Tree* tree_try(Tree* t, void* data){
    if (t==NULL){
        return tree_insert(t, data);
    }

    int comp = t->compare(t->data, data);
    if (comp == 0){
        return t;
    }
    else if (comp > 0)
        return tree_try(t->right, data);
    else    
        return tree_try(t->left, data);
}

Tree* tree_insert(Tree* t, void* data) {
    if (t->data == NULL){
        t->data = data; 
        return t;
    }
    int comp = t->compare(t->data, data);

    if (comp > 0) {
        if(t->right)
            t->right = tree_insert(t->right, data);
        else
            t->right = new_tree(t->compare, data);
    } else if (comp < 0) {
        if(t->left)
            t->left = tree_insert(t->left, data);
        else
            t->left = new_tree(t->compare, data);
    } else {
        // Duplicate keys are not allowed
        return t;
    }

    // Update height
    t->height = 1 + (get_height(t->left) > get_height(t->right) ? get_height(t->left) : get_height(t->right));

    // Balance the tree
    return balance(t);
}


void delete_tree(Tree* t){
    if (t != NULL){
        delete_tree(t->left);
        delete_tree(t->right);
        free(t);
    }
}

// Helper: Count the nodes in the tree.
int count_nodes(Tree* t) {
    if (t == NULL)
        return 0;
    return 1 + count_nodes(t->left) + count_nodes(t->right);
}

// Returns a health score between 0 and 1 for the tree.
// 1 means the tree's height is as low as possible (ideal balance),
// while lower values indicate more imbalance (greater height than ideal).
double tree_health(Tree* t) {
    if (t == NULL)
        return 1.0;  // An empty tree is "healthy" by definition.
    
    int n = count_nodes(t);
    int h_actual = get_height(t);
    
    // For a perfectly balanced (complete) binary tree, the minimum height is:
    double h_ideal = ceil(log2(n + 1));
    
    double ratio = h_ideal / (double)h_actual;
    if (ratio > 1.0)
        ratio = 1.0;  // Cap the health at 1.0
    return ratio;
}

Tree* tree_remove(Tree* t, void* data) {
    if (t == NULL) {
        return NULL;
    }

    int comp = t->compare(data, t->data);

    // Locate node to remove
    if (comp < 0) {
        t->left = tree_remove(t->left, data);
    } else if (comp > 0) {
        t->right = tree_remove(t->right, data);
    } else {
        // Node with one or no child
        if (t->left == NULL) {
            Tree* temp = t->right;
            free(t);
            return temp;
        } else if (t->right == NULL) {
            Tree* temp = t->left;
            free(t);
            return temp;
        }

        // Node with two children: Get inorder successor (smallest in right subtree)
        Tree* successor = t->right;
        while (successor->left != NULL) {
            successor = successor->left;
        }

        // Copy successor data to this node
        t->data = successor->data;

        // Remove successor
        t->right = tree_remove(t->right, successor->data);
    }

    // Update height
    t->height = 1 + (get_height(t->left) > get_height(t->right) ? get_height(t->left) : get_height(t->right));

    // Rebalance and return
    return balance(t);
}
