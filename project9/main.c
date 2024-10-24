#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t MTX = PTHREAD_MUTEX_INITIALIZER;

int checkThread(int val, const char *msg) {
    if (val > 0)
    {
        errno  = val;
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void* player1() { 

}

void* player2() {

}

void* ref() {

    for(int i = 0; i < 10; i++) {

    }
}

int main() {

    void (*funcArray[])(void) = {ref, player1, player2};
    pthread_t thr[3];

    for(int i = 0; i < 3; i++) {
        checkThread(pthread_create(&thr[i], NULL, funcArray[i], NULL), "pthread_create");
        checkThread(pthread_join(thr[i], NULL), "pthread_join");
    }


    return 0;
}