#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//basic error handler for reading/writing failures
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main(int argc, char* argv[]) {

    int rd; //read file
    int bytes; //bytes whether we get a -1, 0, or a file descriptor
    float val; //where all 20 bytes are stored for each iteration of our while loop

    //the file where we're reading the raw acceleration data from
    rd = checkError(open("data.dat", O_RDONLY), "Failed to open read file");

    //keep reading the file until there is no more data left
    while ((bytes = read(rd, &val, sizeof(float))) > 0) {


        printf("value: %f\n", val);
    }

    //close read file
    close(rd);

    exit(EXIT_SUCCESS);
    return 0;
}