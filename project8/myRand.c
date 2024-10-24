#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

int main() {
    srand(time(NULL));

    char filename[20];
    int fd;
    int bytes;
    int value;
    int X;

    X = rand() % 256;

    //Use string interpolation to add our number from X into the filename
    sprintf(filename, "data%d.dat", X);

    fd = checkError(open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH), "Failed to open dataX.dat");

    for(int i = 0; i < 60; i++) {
        value = rand() % 101;

        bytes = checkError(bytes = write(fd, &value, sizeof(value)), "Failed to write to dataX.dat");
    }

    close(fd);
    exit(X);
}