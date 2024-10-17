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
int player1Ready;
int player2Ready;
int player1Guess;
int player2Guess;
int target;
int player1Wins = 0;
int player2Wins = 0;
pid_t player1;
pid_t player2;
int sigReceived = 0;
int resetGame = 0;

//Check error function
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

void parentSignalHandler(int sig) {

    if(sig == SIGUSR1)
        player1Ready = 1; 

    if(sig == SIGUSR2)
        player2Ready = 1; 

    if(sig == SIGINT) {
        kill(player1, SIGTERM);
        kill(player2, SIGTERM);
    }

    if(sig == SIGCHLD)
        wait(NULL);
}

void setupParentSigactions() {
    struct sigaction sa;

    sa.sa_handler = parentSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = parentSignalHandler;
    sigaction(SIGUSR2, &sa, NULL);

    sa.sa_handler = parentSignalHandler;
    sigaction(SIGCHLD, &sa, NULL);
}

void childSignalHandler(int sig) {

    if(sig == SIGUSR1)
        sigReceived = SIGUSR1;

    if(sig == SIGUSR2)
        sigReceived = SIGUSR2;

    if(sig == SIGINT)
        resetGame = 1;

    if(sig == SIGTERM)
        exit(0);
}

void setupChildSigactions() {
    struct sigaction sa;

    sa.sa_handler = childSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = childSignalHandler;
    sigaction(SIGUSR2, &sa, NULL);

    sa.sa_handler = childSignalHandler;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = childSignalHandler;
    sigaction(SIGTERM, &sa, NULL);
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
    srand(time(NULL));
    setupChildSigactions();

    pause();

    while (1) {

        int min = 0;
        int max = MAX_NUMBER;

        kill(getppid(), SIGUSR1);

        while (1) {
            sigReceived = 0;
            resetGame = 0;

            int guess = (min + max) / 2;

            writeAnswerToFile(1, guess);
            sleep(1);

            kill(getppid(), SIGUSR1);
            
            pause();

            if (sigReceived == SIGUSR1)
                min = guess; //Low

            else if (sigReceived == SIGUSR2)
                max = guess; //High

            else if (sigReceived == SIGINT)
                break; //Correct
        }
    }

    exit(0);
}

void player2Logic() {
    srand(time(NULL));
    setupChildSigactions();

    pause();

    while (1) {

        int min = 0;
        int max = MAX_NUMBER;

        kill(getppid(), SIGUSR2);

        while (1) {
            sigReceived = 0;
            resetGame = 0;

            int guess = rand() % (max - min) + min;

            writeAnswerToFile(2, guess);
            sleep(1);

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

    exit(0);
}

void ref() {
    int game;

    struct sigaction sa;

    sa.sa_handler = parentSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    printf("Preparing the game...\n");
    sleep(5);

    kill(player1, SIGUSR1);
    kill(player2, SIGUSR2);

    for(game = 0; game < MAX_ROUNDS; game++) {

        printf("Starting game %d\n", game + 1);

        while (!player1Ready || !player2Ready) {
            pause();
        }

        player1Ready = 0;
        player2Ready = 0;

        printf("Both players are ready for game %d\n", game + 1);

        printf("Player 1 wins = %d, Player 2 wins = %d\n\n", player1Wins, player2Wins);

        target = rand() % MAX_NUMBER;
        printf("Target number is: %d\n", target);

        while (1) {

            while (!player1Ready || !player2Ready) {
                pause();
            }

            int guess1 = readAnswerFromFile(1);
            int guess2 = readAnswerFromFile(2);

            printf("Player 1 guessed: %d\n", guess1);
            printf("Player 2 guessed: %d\n\n", guess2);

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

            sleep(2);
        }

        kill(player1, SIGINT);
        kill(player2, SIGINT);
    }

    printf("Final results: Player 1 wins = %d, Player 2 wins = %d\n", player1Wins, player2Wins);
}

int main() {
    player1 = fork();

    checkError(player1, "Player 1 failed to run");

    if(player1 == 0)
        player1Logic();

    player2 = fork();

    checkError(player2, "Player 2 failed to run");

    if(player2 == 0)
        player2Logic();

    setupParentSigactions();
    ref();
}

/*
I hate this project.

I have it all coded, but I've spent 6 hours trying to fix all these signal bugs.
This project has destroyed my soul.

Whatever grade I get, I don't care. Because I hate this project.
*/