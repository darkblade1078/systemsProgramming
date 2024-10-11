#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

#define MAX_ROUNDS 10 //How many rounds we have
#define MAX_NUMBER 101 //Max number number RNG will create

//Globals
int player1Guess;
int player2Guess;
int target;
int player1Wins = 0;
int player2Wins = 0;
pid_t player1;
pid_t player2;
int sigReceived = 0;

//Check error function
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

//handle signal from SIGTERM
void sigtermHandler(int sig) {
    printf("Killing process %d\n", getpid());
    exit(0);
}

//handle signal from SIGUSR1
void sigusr1Handler(int sig) {
    sigReceived = SIGUSR1;
}

//handle signal from SIGUSR2
void sigusr2Handler(int sig) {
    sigReceived = SIGUSR2;
}

//handle signal from SIGCHLD
void sigchldHandler(int sig) {
    wait(NULL);
}

//handle signal from SIGINT
void sigintHandler(int sig) {
    kill(player1, SIGTERM);
    kill(player2, SIGTERM);
    exit(0);
}

int readAnswerFromFile(int child) {
    
    int rd;
    int value = 0;
    char filename[50];
    char openError[50];
    char readError[50];

    sprintf(filename, "data/player%d.dat", child);
    sprintf(openError, "Failed to open player%d.dat", child);
    sprintf(readError, "Failed to read from player%d.dat", child);

    rd = checkError(open(filename, O_RDONLY, S_IRUSR), openError);

    checkError(read(rd, &value, sizeof(int)), readError);

    close(rd);

    return value;
}

void writeAnswerToFile(int child, int value) {
    
    char filename[30];
    sprintf(filename, "./data/player%d.dat", child);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    checkError(fd, "Failed to open player file for writing");
    checkError(write(fd, &value, sizeof(value)), "Failed to write guess to file");

    close(fd); 
}

void player1Logic() {
    srand(time(NULL) ^ (getpid()<<16));

    int min = 0;
    int max = MAX_NUMBER;

    while (1) {
        pause();

        int guess = (min + max) / 2;

        writeAnswerToFile(1, guess);

        kill(getppid(), SIGUSR1);
        pause();

        if (sigReceived == SIGUSR1)
            min = guess; //Low

        else if (sigReceived == SIGUSR2)
            max = guess; //High

        else if (sigReceived == SIGINT)
            break; //Correct
    }

    exit(0);
}

void player2Logic() {
    srand(time(NULL) ^ (getpid()<<16));

    int min = 0;
    int max = MAX_NUMBER;

    while (1) {
        pause();

        int guess = rand() % (max - min) + min;

        writeAnswerToFile(2, guess);

        kill(getppid(), SIGUSR2);
        pause();

        if (sigReceived == SIGUSR1)
            min = guess; //Low

        else if (sigReceived == SIGUSR2)
            max = guess; //High

        else if (sigReceived == SIGINT)
            break; //Correct
    }
}

void ref() {
    int game;

    printf("Preparing the game...\n");

    sleep(5);

    for(game = 0; game < MAX_ROUNDS; game++) {
        printf("Starting game %d\n", game + 1);

        target = rand() % MAX_NUMBER;

        kill(player1, SIGUSR1);
        kill(player2, SIGUSR1);

        while (1) {
            pause();

            int guess1 = readAnswerFromFile(1);
            int guess2 = readAnswerFromFile(2);

            printf("Player 1 guessed: %d\n", guess1);
            printf("Player 2 guessed: %d\n", guess2);

            if(guess1 == target) {
                player1Wins++;
                printf("Player 1 guessed the number first!!!\n");
                break;
            }
            else if(guess2 == target) {
                player2Wins++;
                printf("Player 2 guessed the number first!!!\n");
                break;
            }

            if(guess1 < target) 
                kill(player1, SIGUSR1);
            else
                kill(player1, SIGUSR2);

            if(guess2 < target) 
                kill(player2, SIGUSR1);
            else
                kill(player2, SIGUSR2);
        }

        sleep(2);
    }

    printf("Final results: Player 1 wins = %d, Player 2 wins = %d\n", player1Wins, player2Wins);
}

int main() {
    srand(time(NULL)); //Set SEED to NULL

    signal(SIGUSR1, sigusr1Handler);
    signal(SIGUSR2, sigusr2Handler);
    signal(SIGCHLD, sigchldHandler);
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigtermHandler);

    player1 = fork();

    checkError(player1, "Player 1 failed to run");

    if(player1 == 0)
        player1Logic();

    player2 = fork();

    checkError(player2, "Player 2 failed to run");

    if(player2 == 0)
        player2Logic();

    ref();
}