#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {

    printf("works\n");

    return 0;
}