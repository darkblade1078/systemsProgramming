#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int global;

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

void* startThread1(void* arg) {
    struct timespec ts = {5, 0};
    nanosleep(&ts, NULL);
    printf("Thread 1: %d\n", global);
    global = 8;
    nanosleep(&ts, NULL);
    printf("Thread 1: %d\n", global);
    return NULL;
}

void* startThread2(void* arg) {
    global = 2;
    struct timespec ts = {6, 0};
    printf("Thread 2: %d\n", global);
    nanosleep(&ts, NULL);
    printf("Thread 2: %d\n", global);
    global = 12;
    return NULL;
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

int main() {

    pthread_t thr[2];

    checkThread(pthread_create(&thr[0], NULL, startThread1, NULL), "Thread 1");

    checkThread(pthread_create(&thr[1], NULL, startThread2, NULL), "thread 2");

    for(int i = 0; i < 2; i++) {
        checkThread(pthread_join(thr[i], NULL), "Join Thread");
    }

    return 0;
}