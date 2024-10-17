#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int glob;
pthread_t thr[2];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

int checkThread(int val, const char* msg) {

    if(val != 0) {
        errno = val;
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return 0;
}

void signalHandler(int sig) {

    char* msg = "Exit (y/N)";
    char yorn[5];
    int rdNum;

    if(sig == SIGINT) {
        write(STDOUT_FILENO, msg, strlen(msg));
        rdNum = read(STDIN_FILENO, yorn, 5);

        if(rdNum == 2 && (yorn[0] == 'Y' || yorn[0] == 'y')) exit(EXIT_SUCCESS);
    }
}

void* startThread(void* arg) {
    int *cnt = (int*) arg;
    int i = 0;
    int loc;
    for(i = 0; i < *cnt; i++) {
        checkThread(pthread_mutex_lock(&mtx), "lock mutex");
        glob++;
        checkThread(pthread_mutex_unlock(&mtx), "unlock mutex");
    }

    return NULL;
}

int main() {
    int i = 0;

    int val = 200000;
    void* arg = (void *) &val;

    for(i = 0; i < 2; i++) {
            checkThread(pthread_create(&thr[0], NULL, startThread, NULL), "Thread");
    }

    printf("%d", glob);
}