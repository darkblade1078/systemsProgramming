#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

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

int main () {

    int bytes;
    int rd;
    int i = 0;
    double valz[3];

    struct timespec time;
    time.tv_sec = 1;
    time.tv_nsec = 0;
    
    
    int outside_range = 0;
    int inside_range = 0;

    rd = checkError(open("values/angl.dat", O_RDONLY), "Failed to open angl.dat");

    while ((bytes = read(rd, valz, sizeof(double) * 3)) > 0) {

        printf("Packet: %d\n", ++i);

        printf("Roll: %lf\n", valz[0]);

        if ((valz[0] >= -20) & (valz[0] <= 20)) {
            printf("Inside the range\n\n");
            inside_range++;
        }
        else {
            printf("Outside the range\n\n");
            outside_range++;
        }

        printf("Pitch: %lf\n", valz[1]);

        if ((valz[1] >= -20) & (valz[1] <= 20)) {
            printf("Inside the range\n\n");
            inside_range++;
        }
        else {
            printf("Outside the range\n\n");
            outside_range++;
        }

        printf("Yaw: %lf\n", valz[2]);

        if ((valz[2] >= -20) & (valz[2] <= 20)) {
            printf("Inside the range\n\n");
            inside_range++;
        }
        else {
            printf("Outside the range\n\n");
            outside_range++;
        }
        
        printf("\n\n waiting one second...\n\n");
        nanosleep(&time, NULL);
    }
    
    printf("%d entries are inside the range\n", inside_range);
    printf("%d entries are outside the range\n", outside_range);
}