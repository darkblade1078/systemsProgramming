#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void sigChldHandler(int sig) {
    print("Recieved a signal from SIGCHLD\n");
}

int main() {
    pid_t pid;
    int status;
    int rd;
    int bytes;
    int value;

    //To prevent SIGCHLD from killing the program (And to see the signal get recieved)
    signal(SIGCHLD, sigChldHandler);

    pid = fork();

    checkError(pid, "Failed to create child");

    //Run exec
    if(pid == 0) {
        checkError(execve("./myRand", NULL), "Failed to run exec on myRand");
    }
    else {

        //Wait for the child to finish myRand.c and return status
        wait(&status);
        
        if (WIFEXITED(status)) {

            int values[60];
            int sum;
            double avg;

            int X = WEXITSTATUS(status);
            printf("myRand returned: %d\n", X);

            //Use string interpolation to add our number from X into the filename
            char filename[20];
            sprintf(filename, "data%d.dat", X);

            rd = checkError(open("dataX.dat", O_RDONLY, S_IRUSR), "Failed to open dataX.dat");

            checkError(bytes = read(rd, values, sizeof(int) * 60), "Failed to read dataX.dat");

            for(int i = 0; i < 60; i++) {
                printf("Number from file: %d\n", value[i]);
                sum += value[i];
                printf("Sum: %d\n", sum);
            }

            avg = sum / 60.0;
            printf("Average: %lf\n", avg);

            close(rd);

            checkError(unlink(filename), "Failed to unlink dataX.dat");

            printf("Deleted file: %s\n", filename);
        }
    }

    return 0;
}