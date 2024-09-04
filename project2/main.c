#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#define PACKET_SIZE 20  //each packet is 20 bytes
#define HEADER_SIZE 2   //first 2 bytes are the header and flag and are not needed

//basic error handler for reading/writing failures
int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

//data / 32768.0 * 16.0
float convertRawAccelerationData(float value) {
    return value / 32768.0 * 16.0;
}

//data / 32768.0 * 2000.0
float convertRawAngularData(float value) {
    return value / 32768.0 * 2000.0;
}

//data / 32768.0 * 180.0
float convertRawAngleData(float value) {
    return value / 32768.0 * 180.0;
}

int main(int argc, char* argv[]) {

    int fd; //write file
    int rd; //read file
    int bytes; //bytes whether we get a -1, 0, or a file descriptor
    unsigned char valz[PACKET_SIZE]; //where all 20 bytes are stored for each iteration of our while loop
    float rawData; //where we store our raw data after shifting bits
    float convertedData; //where we store our converted data after using the proper conversion function

    //the file where we're reading the raw acceleration data from
    rd = checkError(open("raw.dat", O_RDONLY, S_IRUSR), "Failed to open read file");

    //the file we are sending the converted acceleration data to.
    fd = checkError(open("data.dat", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "Failed to open write file");

    //keep reading the file until there is no more data left
    while ((bytes = read(rd, valz, PACKET_SIZE)) > 0) {

        /*
            since our first 2 bytes are the header and flag bytes,
            we don't need them and can skip them all together
            hence why we are starting at index 2
        */
        for(int i = HEADER_SIZE; i < PACKET_SIZE; i += 2) {

            //(H | (L << 8))
            rawData = ((short)(valz[i + 1] << 8) | valz[i]);

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
                   printf("%s: %f\n", "Ax", convertedData);
                    break;

                //Ay
                case 4:
                   convertedData = convertRawAccelerationData(rawData);
                   printf("%s: %f\n", "Ay", convertedData);
                    break;

                //Az
                case 6:
                   convertedData = convertRawAccelerationData(rawData);
                   printf("%s: %f\n", "Az", convertedData);
                    break;
                
                //Wx
                case 8:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %f\n", "Wx", convertedData);
                    break;

                //Wy
                case 10:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %f\n", "Wy", convertedData);
                    break;

                //Wz
                case 12:
                   convertedData = convertRawAngularData(rawData);
                   printf("%s: %f\n", "Wz", convertedData);
                    break;

                //Roll
                case 14:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %f\n", "Roll", convertedData);
                    break;

                //Pitch
                case 16:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %f\n", "Pitch", convertedData);
                    break;

                //Yaw
                case 18:
                   convertedData = convertRawAngleData(rawData);
                   printf("%s: %f\n\n", "Yaw", convertedData);
                    break;
            }

            //write the converted data to our data.dat file
            bytes = checkError(bytes = write(fd, &convertedData, sizeof(convertedData)), "Failed to write to file");
        }
    }

    //close both read and write files
    close(rd);
    close(fd);

    exit(EXIT_SUCCESS);
    return 0;
}