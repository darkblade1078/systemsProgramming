#ifndef BST_H
#define BST_H

#include <stdbool.h>

typedef struct Node {
    char *value;
    struct Node* left;
    struct Node* right;
} Node;

Node* new_node(char* value);
Node* new_node_null();

typedef struct BST {
    Node* root;
} BST;

BST* new_bst();
void copy_bst();
void free_bst();

void insert_bst(BST* bst, char* value);
Node* insert_node(Node* node, char* value);

bool search(Node* node, const char* value);
void postOrder(Node* node);

#endif