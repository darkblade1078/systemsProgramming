#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

int breakPause = 0;

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
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

    if(sig == SIGALRM) {
        breakPause = 1;
    }
}

int main() {

    struct sigaction sa;
    struct itimerval it;

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    it.it_value.tv_sec = 10;
    it.it_value.tv_usec = 0;
    it.it_internal.tv_sec = 10;
    it.it_internal.tv_usec = 0;

    checkError(sigaction(SIGINT, &sa, NULL), "sigaction");
    checkError(sigaction(SIGALRM, &sa, NULL), "sigaction");

    checkError(setitmer(ITIMER_REAL, &it, NULL), "settimer");

    printf("We are pausing for a moment\n");

    while (1)
    {
        pause();
        
        if(breakPause) 
            break;
    }

    printf("Done pausing...");
    

    return 0;
}