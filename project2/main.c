#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main(int argc, char* argv[]) {

    int fd;
    int rd;
    int i = 0;
    int size = 20;
    double valz = 1.0;
    int bytes = 0;

    rd = checkError(open("raw.dat", O_RDONLY), "Open for Read");
    fd = checkError(open("data.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Open for Write");

    for(i = 0; i < size; i++) {
        bytes = checkError(read(rd, &valz, sizeof(double)), "read");
        printf("%lf\n", valz);
        bytes = checkError(write(fd, &valz, sizeof(double)), "write");
    }

    close(rd);
    close(fd);

    exit(EXIT_SUCCESS);
    return 0;
}