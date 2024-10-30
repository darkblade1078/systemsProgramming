#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#define ROUNDS 10 //How many rounds there are 
#define MAX_NUMBER 100 //The max number the program will generate

//Thread variables
pthread_t thr[3];
pthread_mutex_t mtx[3];
pthread_cond_t cnd[3];

//game variables
int guess[2];
int cmp[2];
int rdy[4];
int wins[2];

int checkThread(int val, const char* msg) {

    if(val > 0) {
        errno = val;
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}


void* player1() {

    while(1) {
        int min = 0;
        int max = 100;

        pthread_mutex_lock(&mtx[2]);

        while (rdy[2] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }

        rdy[2] = 0;

        pthread_mutex_unlock(&mtx[2]);

        while (1) {

            guess[0] = rand() % (max - min) + min;

            pthread_mutex_lock(&mtx[0]);

            while (!rdy[0])
                pthread_cond_wait(&cnd[0], &mtx[0]);

            pthread_cond_signal(&cnd[0]);
            
            if(cmp[0] < 0)
                min = guess[0];
            else if(cmp[0] > 0)
                max = guess[0];
            else if (cmp[0] == 0) {
                pthread_mutex_unlock(&mtx[0]);
                break;
            }
            pthread_mutex_unlock(&mtx[0]);
        }
    }
}

void* player2() {

    while(1) {
        int min = 0;
        int max = 100;

        pthread_mutex_lock(&mtx[2]);

        while (rdy[2] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }

        rdy[2] = 0;

        pthread_mutex_unlock(&mtx[2]);

        while (1) {

            guess[1] = rand() % (max - min) + min;

            pthread_mutex_lock(&mtx[1]);

            while (!rdy[1])
                pthread_cond_wait(&cnd[1], &mtx[1]);

            pthread_cond_signal(&cnd[1]);
            
            if(cmp[1] < 0)
                min = guess[1];
            else if(cmp[1] > 0)
                max = guess[1];
            else if (cmp[1] == 0) {
                pthread_mutex_unlock(&mtx[1]);
                break;
            }
            pthread_mutex_unlock(&mtx[1]);
        }
    }
}

void* ref() {

    for(int i = 0; i < ROUNDS; i++) {

        pthread_mutex_lock(&mtx[2]);

        int target = (rand() % 100) + 1;

        rdy[0] = 1;
        rdy[1] = 1;

        pthread_cond_broadcast(&cnd[2]);
        pthread_mutex_unlock(&mtx[2]);

        printf("Game %d: player 1 = %d wins, player 2 = %d wins\n\n", i + 1, wins[0], wins[1]);

        while (1) {
            sleep(1);

            pthread_mutex_lock(&mtx[0]);
            pthread_mutex_lock(&mtx[1]);

            rdy[0] = 0;
            rdy[1] = 0;

            cmp[0] = guess[0] - target;
            cmp[1] = guess[1] - target;

            rdy[0] = 1;
            rdy[1] = 1;


            pthread_cond_broadcast(&cnd[0]);
            pthread_cond_broadcast(&cnd[1]);

            pthread_mutex_unlock(&mtx[0]);
            pthread_mutex_unlock(&mtx[1]);

            if(cmp[0] == 0) {
                wins[0]++;
                break;
            }
            else if(cmp[1] == 0) {
                wins[1]++;
                break;
            }
        }

        cmp[0] = 0;
        cmp[1]= 0;
    }

    printf("player 1 wins = %d, player 2 wins = %d\n", wins[0], wins[1]);
    return NULL;
}

int main() {
    srand(time(NULL));

    wins[0] = 0;
    wins[1] = 0;

    for(int i = 0; i < 3; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_cond_init(&cnd[i], NULL);
        rdy[i] = 0;
    }

    checkThread(pthread_create(&thr[0], NULL, player1, NULL), "creating player 1 thread");
    checkThread(pthread_create(&thr[1], NULL, player2, NULL), "creating player 2 thread");
    checkThread(pthread_create(&thr[2], NULL, ref, NULL), "creating ref thread");

    checkThread(pthread_join(thr[2], NULL), "Joining thread");

    return 0;
}