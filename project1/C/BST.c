#include "BST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    since we don't have classes nor contructors.
    the best way i have found to handle this problem,
    is to create two different classes.
    one for a node with a initial value.
    and one for a node with no initial value.
    and we use pointers to link our character array
    and our node.
*/
Node* new_node(char* value) {
    /*
        allocates the new node to memory.
        malloc - allocate memory
        sizeof - the size of the datatype
        Node - the node struct itself
    */
    Node *node = malloc(sizeof(Node));
    node->value = strdup(value);
    node->left = NULL;
    node->right = NULL;

    return node;
}

/*
    since we don't have function overload in C,
    the next best thing we can do is to implement seperate functions.
    there might be a better way using _Generics (c11 and higher)
    of the C compiler and i don't want to mess with that yet.
*/
Node* new_node_null() {

    /*
        allocates the new node to memory.
        malloc - allocate memory
        sizeof - the size of the datatype
        Node - the node struct itself
    */
    Node *node = malloc(sizeof(Node));
    node->value = NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}

//frees every single node connected below the given node
void free_node(Node* node) {

    //ends the recursion process once there are
    //no more nodes left
    if(node == NULL)
        return;

    //use recursion to remove left and right nodes,
    //until all nodes are freed from memory
    free_node(node->left);
    free_node(node->right);

    //frees current node and value from memory.
    free(node->value);
    free(node);
}

BST* new_bst() {
    //allocates the new BST to memory.
    BST *bst = malloc(sizeof(BST));
    bst->root = NULL;

    return bst;
}

//this function is very similar to the free_node function,
//except we are creating a new bst from an old one
void copy_bst(Node* node, BST* bst) {
    if (node == NULL)
        return;

    insert_bst(bst, node->value);
    copy_bst(node->left, bst);
    copy_bst(node->right, bst);
}

//frees the root node and all sub nodes from memory
void free_bst(BST* bst) {
    //runs a recursion function that wipes every node from the root.
    free_node(bst->root);
    bst->root = NULL;
}

//function to call when you want to insert into the BST
void insert_bst(BST* bst, char* value) {
    bst->root = insert_node(bst->root, value);
}

//recursive function that inserts the new node
//and moves around current nodes to new locations
Node* insert_node(Node* node, char* value) {
    if(node == NULL)
        return new_node(value);

    //moves the old node left or right depending on the value of the data
    if (strcmp(value, node->value) < 0) {
        node->left = insert_node(node->left, value);
    }

    else if (strcmp(value, node->value) > 0) {
        node->right = insert_node(node->right, value);
    }

    return node;
}

//function to see if string exists in the BST
bool search(Node* node, const char* value) {
    if (node == NULL)
        return false;

    /*
        these two if statements use a function called string compare.
        string compare will take two strings and return 1 or 0 (true or false).
        how it returns the boolean is based on the contents of the strings as
        it compares each string character by character
    */
    if (strcmp(value, node->value) < 0) 
        return search(node->left, value);

    if (strcmp(value, node->value) > 0)
        return search(node->right, value);

    return true;
}

//this function will traverse the BST and print each node value
//from the left nodes all the way to the nodes on the right
void postOrder(Node* node) {
  if (node == NULL)
    return;

  postOrder(node->left);
  postOrder(node->right);

  printf("%s ", node->value);
}