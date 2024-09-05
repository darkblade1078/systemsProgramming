#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define PACKET_SIZE 9  //each packet is 9 bytes

//basic error handler for reading/writing failures
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {

    int rd; //read file
    int acclDat; //accleration data file
    int anglDat; //angular data file
    int rotaDat; //rotation data file
    int bytes; //bytes whether we get a -1, 0, or a file descriptor and so on
    float valz[PACKET_SIZE]; //where we store the data as we iterate through our loop
    float val; //the data we will use to write into it's proper file

    bytes = checkError(mkdir("values"), "Failed to create values directory");

    //data.dat
    rd = checkError(open("data.dat", O_RDONLY, S_IRUSR), "Failed to open data.dat");

    //accl.dat
    acclDat = checkError(open("values/accl.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Failed to open accl.dat");

    //angl.dat
    anglDat = checkError(open("values/angl.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Failed to open angl.dat");

    //rota.dat
    rotaDat = checkError(open("values/rota.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Failed to open rota.dat");

    //keep reading the file until there is no more data left
    while ((bytes = read(rd, valz, sizeof(float) * PACKET_SIZE)) > 0) {

        for(int i = 0; i < PACKET_SIZE; i++) {

            switch (i)
            {
                //Ax
                case 0:
                   val = valz[i];
                   printf("%s: %f\n", "Ax", val);
                   bytes = checkError(bytes = write(acclDat, &val, sizeof(val)), "Failed to write to accl.dat");
                    break;

                //Ay
                case 1:
                   val = valz[i];
                   printf("%s: %f\n", "Ay", val);
                   bytes = checkError(bytes = write(acclDat, &val, sizeof(val)), "Failed to write to accl.dat");
                    break;

                //Az
                case 2:
                   val = valz[i];
                   printf("%s: %f\n", "Az", val);
                   bytes = checkError(bytes = write(acclDat, &val, sizeof(val)), "Failed to write to accl.dat");
                    break;
                
                //Wx
                case 3:
                   val = valz[i];
                   printf("%s: %f\n", "Wx", val);
                   bytes = checkError(bytes = write(rotaDat, &val, sizeof(val)), "Failed to write to rota.dat");
                    break;

                //Wy
                case 4:
                   val = valz[i];
                   printf("%s: %f\n", "Wy", val);
                   bytes = checkError(bytes = write(rotaDat, &val, sizeof(val)), "Failed to write to rota.dat");
                    break;

                //Wz
                case 5:
                   val = valz[i];
                   printf("%s: %f\n", "Wz", val);
                   bytes = checkError(bytes = write(rotaDat, &val, sizeof(val)), "Failed to write to rota.dat");
                    break;

                //Roll
                case 6:
                    val = valz[i];
                   printf("%s: %f\n", "Roll", val);
                   bytes = checkError(bytes = write(anglDat, &val, sizeof(val)), "Failed to write to angl.dat");
                    break;

                //Pitch
                case 7:
                   val = valz[i];
                   printf("%s: %f\n", "Pitch", val);
                   bytes = checkError(bytes = write(anglDat, &val, sizeof(val)), "Failed to write to angl.dat");
                    break;

                //Yaw
                case 8:
                   val = valz[i];
                   printf("%s: %f\n", "Yaw", val);
                   bytes = checkError(bytes = write(anglDat, &val, sizeof(val)), "Failed to write to angl.dat");
                    break;
            }
        }
    }

    //close all files file
    close(rd);
    close(acclDat);
    close(anglDat);
    close(rotaDat);

    exit(EXIT_SUCCESS);
    return 0;
}