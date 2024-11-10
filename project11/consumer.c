#include <stdio.h>
#include <stdlib.h>
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
    int rd;

    rd = checkError(open(FIFO_PATH, O_RDONLY), "Failed to open fifo");

    while ((bytes = read(rd, buffer, sizeof(buffer))) > 0) {
        printf("consumer: %s\n", buffer);
    }

    close(rd);
    return 0;
}