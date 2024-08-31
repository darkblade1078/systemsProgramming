#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#define PACKET_SIZE 20  //each packet is 20 bytes
#define HEADER_SIZE 2   //first 2 bytes are header and are not needed

//basic error handler for reading/writing failures
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

//((H << 8) | L) / 32768.0 * 16.0
double convertRawAccelerationData(int16_t value) {
    return value / 32768.0 * 16.0;
}

//((H << 8) | L) / 32768.0 * 2000.0
double convertRawAngularData(int16_t value) {
    return value / 32768.0 * 2000.0;
}

//((H << 8) | L) / 32768.0 * 180.0
double convertRawAngleData(int16_t value) {
    return value / 32768.0 * 180.0;
}

int main(int argc, char* argv[]) {

    int fd; //write file
    int rd; //read file
    int bytes; //bytes whether we get a -1, 0, or file descriptor
    uint8_t valz[PACKET_SIZE]; //where all 20 bytes are stored for each iteration of our while loop
    uint16_t rawData; //where we store our raw data after shifting bits
    double convertedData; //where we store our converted data after using the proper conversion function

    //the file where we're reading the raw acceleration data from
    rd = checkError(open("raw.dat", O_RDONLY), "Open for Read");

    //the file we are sending the converted acceleration data to.
    fd = checkError(open("data.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Open for Write");

    //keep reading the file until there is no more data left
    while ((bytes = read(rd, valz, PACKET_SIZE)) > 0) {

        /*
            since our first 2 bytes are header bytes,
            we don't need them and can skip them all together
            hence why we are starting at index 2
        */
        for(int i = HEADER_SIZE; i < PACKET_SIZE; i += 2) {

            //(L | (H << 8))
            rawData = valz[i] | (valz[i + 1] << 8);

            /*
                use a switch case to know which data is being read
                so we can use the proper functions to convert the data
                and print it out with the proper data type
            */
            switch (i)
            {
                //Ax
                case 2:
                   convertedData = convertRawAccelerationData(rawData);
                   printf("%s: %lf\n", "Ax", convertedData);
                    break;

                //Ay
                case 4:
                   convertedData = convertRawAccelerationData(rawData);
                   printf("%s: %lf\n", "Ay", convertedData);
                    break;

                //Az
                case 6:
                   convertedData = convertRawAccelerationData(rawData);
                   printf("%s: %lf\n", "Az", convertedData);
                    break;
                
                //Wx
                case 8:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %lf\n", "Wx", convertedData);
                    break;

                //Wy
                case 10:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %lf\n", "Wy", convertedData);
                    break;

                //Wz
                case 12:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %lf\n", "Wz", convertedData);
                    break;

                //Roll
                case 14:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %lf\n", "Roll", convertedData);
                    break;

                //Pitch
                case 16:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %lf\n", "Pitch", convertedData);
                    break;

                //Yaw
                case 18:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %lf\n\n", "Yaw", convertedData);
                    break;
            }

            //write the converted data to our data.dat file
            bytes = checkError(write(fd, &convertedData, sizeof(double)), "Failed to write to file");
        }
    }

    //close both read and write files
    close(rd);
    close(fd);

    exit(EXIT_SUCCESS);
    return 0;
}