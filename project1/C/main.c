#include "BST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    //initialized BST and our character array 
    BST* store = new_bst(); //our constructor essentially
    char* word = malloc(1);

    //print instructions
    printf("Please enter a series of words to store in a BST\n");
    printf("Type 'STOP' to end the series and output the BST\n");
    printf("In postorder\n");

    while (1) {

        //use scanf to get input from the user
        printf("Word = ");
        scanf("%s", word);

        //use sting compare to see if word & STOP match
        if (strcmp(word, "STOP") == 0)
            break;

        //insert word into the BST
        insert_bst(store, word);
    }

    //output the post order of the BST
    printf("The BST ouput in postorder is \n");
    postOrder(store->root);
    printf("\n");

    //frees every single node in the BST from memory
    free_bst(store);

  return 1;
}