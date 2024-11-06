#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/myfifo"
#define BUFFER_SIZE 256

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {

    char buffer[BUFFER_SIZE];
    int bytes;
    int fd;

    //if there is no fifo, create one
    if(access(FIFO_PATH, F_OK) == -1)
        bytes = checkError(mkfifo(FIFO_PATH, 0666), "Failed to create fifo");

    fd = checkError(open(FIFO_PATH, O_WRONLY), "Failed to open fifo");

    printf("Please enter text\ntext: ");

    fflush(stdout); //used chatGPT to fix my output
    bytes = checkError(read(STDIN_FILENO, buffer, BUFFER_SIZE), "Failed to read data"); //read input from user

    checkError(write(fd, buffer, strlen(buffer)), "Failed to write to fifo");

    close(fd);

    return 0;
}