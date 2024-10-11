#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

pid_t pid = -1; //global variable to check and see if a child is active

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

void signalHandler(int sig) {

    char* msg = "\nExit (y/N)";
    char yorn[5];
    int rdNum;

    if(sig == SIGINT) {

        write(STDOUT_FILENO, msg, strlen(msg));
        rdNum = read(STDIN_FILENO, yorn, 5);

        if(rdNum == 2 && (yorn[0] == 'Y' || yorn[0] == 'y')) {

            if (pid > 0) {

                kill(pid, SIGTERM);
                printf("Killing child from SIGINT with ID of: %d\n", pid);
            }

            exit(EXIT_SUCCESS);
        }
    }

    if(sig == SIGUSR1)
        printf("Warning! roll outside of bounds\n\n");

    if(sig == SIGUSR2)
        printf("Warning! pitch outside of bounds\n\n");

    if(sig == SIGTERM) {
        printf("SIGTERM: Child terminating\n");
        exit(0);
    }
}

void childSignalHandler() {

    pid_t pid2;
    int status;

    while ((pid2 = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child %d terminated\n", pid2);

        if (pid2 == pid) 
            exit(EXIT_SUCCESS); //could figure out why my parent pid wouldn't update, so i decided to cheese it with this after the child is killed
    }
}

void parent(pid_t pid) {

    printf("Parent ID: %d\n", getpid());
    printf("Child ID: %d\n\n", pid);

    while (1) {
        pause(); //wait for signal
    }
}

void child() {

    int rd;
    int bytes;
    double valz[3];

    sigset_t blockSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGINT);
    sigprocmask(SIG_BLOCK, &blockSet, NULL);

    rd = checkError(open("angl.dat", O_RDONLY), "Failed to open angl.dat");

    signal(SIGTERM, signalHandler); //SIGTERM (handles parent dying)

    while((bytes = read(rd, valz, sizeof(double) * 3)) > 0) {

        printf("Roll Value: %lf\n\n", valz[0]);
        printf("Pitch Value: %lf\n\n", valz[1]);
        
        if (valz[0] < -20.0 || valz[0] > 20.0)
            kill(getppid(), SIGUSR1);

        if (valz[2] < -20.0 || valz[2] > 20.0)
            kill(getppid(), SIGUSR2);

        printf("Line read\n\n");

        //sleep(1);
    }
    
    close(rd);
    exit(0);
}

int main() {

    struct sigaction sa;

    //handle SIGCHLD
    sa.sa_handler = childSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // Ensure interrupted system calls are restarted
    sigaction(SIGCHLD, &sa, NULL);

    //handle SIGUSR1 & 2
    signal(SIGUSR1, signalHandler); //SIGUSR1
    signal(SIGUSR2, signalHandler); //SIGUSR2

    //handle SIGINT
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    pid = fork(); //create child

    checkError(pid, "Fork Failed"); //check if fork failed

    if(pid == 0)
        child(); //goto child if pid is a child
    else
        parent(pid); //otherwise run the parent function

    return 0;
}