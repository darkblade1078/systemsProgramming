#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void child(int readPipe) {

    int bytes;
    char buffer[BUFFER_SIZE];

    //read data from our pipe
    while ((bytes = read(readPipe, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes] = '\0';
        printf("Child: %s\n", buffer);
    }

    close(readPipe); //close read pipe

    exit(0);
}

void parent(int writePipe) {

    int bytes;
    char buffer[BUFFER_SIZE];

    printf("Please enter text at the parent\n");

    printf("Parent: ");
    fflush(stdout); //used chatGPT to fix my output
    bytes = checkError(read(STDIN_FILENO, buffer, BUFFER_SIZE), "Failed to read data"); //read input from user
    
    checkError(write(writePipe, buffer, bytes), "Failed to write data"); //write user input into our pipe

    close(writePipe); //close write pipe
}

int main() {

    int pipefd[2];
    pid_t pid;

    checkError(pipe(pipefd), "Failed to create pipes"); //create pipes

    pid = fork();

    checkError(pid, "Failed to create child");

    if(pid == 0) {
        close(pipefd[1]); //close write pipe
        child(pipefd[0]);
    }
    else {
        close(pipefd[0]); //close read pipe
        parent(pipefd[1]);
    }

    return 0;
}