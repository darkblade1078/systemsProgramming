#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024 // just a 1024 byte buffer
#define QUESTION_ANSWER_SIZE 15 // 15 questions and answers

int timedOut = 0; //the time out flag

int checkError(int val, const char* msg) {

    if(val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return val;
}

void signalHandler(int sig) {

    char* msg = "\nExit (y/N)";
    char yorn[5];
    int rdNum;

    if(sig == SIGINT) {
        write(STDOUT_FILENO, msg, strlen(msg));
        rdNum = read(STDIN_FILENO, yorn, 5);

        if(rdNum == 2 && (yorn[0] == 'Y' || yorn[0] == 'y')) exit(EXIT_SUCCESS);
    }

    if (sig == SIGALRM) {
        timedOut = 1; 
        write(STDOUT_FILENO, "Timed out... going to next question\n\n", 36);
    }
}

//free an array from memory
void freeArray(char** array) {

    for (int i = 0; i < QUESTION_ANSWER_SIZE; i++)
        free(array[i]);

    free(array);
}

//get our questions from the quest.txt file and return that data as a char array pointer
char** getDataFromFile(char* filePath) {

    int bytes; // bytes being read
    int rd; // read descriptor
    int line_pos = 0; // position in the current line of the text file
    int iteration = 0; // the iteration of the questions double array pointer (this is so cursed)
    char *line = malloc(BUFFER_SIZE); // a line pointer to store each char until the line has been fully read
    char buffer[BUFFER_SIZE]; // our buffer for storing data from the quest.txt file

    /*

        https://stackoverflow.com/questions/14808908/pointer-to-2d-arrays-in-c

        So I wanted to make a function to handle getting the data and storing them in an array.
        Sadly, C is special when it comes to strings. So instead of a simple array, I would have to implement a char array that points to the strings I want.
        After reading the stackoverflow link above and trying to get my own janky version of it working for arrays all in one line of code.
        I eventually was able to get my janky version to run with no compile errors (just a lot of warnings), but it wouldn't read data because it was uninitialzed.
        After 30 minutes of googling, crying, and dumbfounded, I reluctantly went to chatGPT, gave my janky char array pointer and asked it how do i initialize this?
        And that's how I got this line of code, "questions[iteration] = (char*)malloc((strlen(line) + 1) * sizeof(char));", and after some tweaking to make it work in a for loop,
        That's how I got this char array pointer working, initialized, and usable. I hope I never have to do this again for at least 6 months.
    */
    char** data = (char**)malloc(QUESTION_ANSWER_SIZE * sizeof(char*)); // our mega cursed char array pointer

    rd = checkError(open(filePath, O_RDONLY), "Failed to open quest.txt");

    while ((bytes = read(rd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes; i++) {
            if (buffer[i] == '\n') {
                line[line_pos] = '\0'; //null terminate the string
                line_pos = 0; //reset our line position to 0

                // cast type our pointer index while using malloc to allocate memory to that pointer index
                data[iteration] = (char*)malloc((strlen(line) + 1) * sizeof(char));

                //use strcpy to copy the string as a pointer to our pointer
                strcpy(data[iteration], line);

                //incriment our iteration
                iteration++;
            } 
            else 
                line[line_pos++] = buffer[i]; //go to the next character in the line
        }
    }

    //if there is characters left to read when the while loop finishes
    if(line_pos > 0) {

        line[line_pos] = '\0'; //null terminate the string

        //allocate memory and copy the last line to the array
        data[iteration] = (char*)malloc((strlen(line) + 1) * sizeof(char));
        strcpy(data[iteration], line);
        iteration++;
    }

    close(rd);
    free(line);

    return data;
}

void setTimer() {
    struct itimerval it;
    it.it_value.tv_sec = 15;
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    checkError(setitimer(ITIMER_REAL, &it, NULL), "setitimer");
}

int main () {

    printf("----------------------------\n");
    printf("|       My Quiz Game       |\n");
    printf("----------------------------\n\n\n");

    printf("This is a quiz game with %d questions for you to answer.\n", QUESTION_ANSWER_SIZE);
    printf("Each question will give you %d seconds to answer them.\n", QUESTION_ANSWER_SIZE);
    printf("All the answers are in lowercase by the way.\n");
    printf("--------------------------------------------------------\n\n");

    printf("Press Enter to continue......");

    getchar();

    printf("\n\n\n");

    char** questions = getDataFromFile("data/quest.txt"); //get our questions
    char** answers = getDataFromFile("data/ans.txt"); //get our answers
    char userAnswer[BUFFER_SIZE];

    int points = 0;

    struct sigaction sa;

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    checkError(sigaction(SIGINT, &sa, NULL), "SIGINT");
    checkError(sigaction(SIGALRM, &sa, NULL), "SIGALRM");

    for(int i = 0; i < QUESTION_ANSWER_SIZE; i++) {
        setTimer(); //start the timer
        timedOut = 0; 

        printf("Question %d: %s\n", i + 1, questions[i]);

        read(STDIN_FILENO, userAnswer, sizeof(userAnswer) - 1);

        if (timedOut)
            continue;

        //idk why strcmp keeps returning a -3 even after null terminating /n and /r, but +3 works!!!!
        if ((strcmp(userAnswer, answers[i]) + 3) == 0) {
            printf("Correct!\n\n");
            points++;
        } 
        else 
            printf("Incorrect. The correct answer was: %s\n\n", answers[i]);
    }

    printf("You got %d/%d questions correct!!!\n", points, QUESTION_ANSWER_SIZE);

    //free our arrays
    freeArray(questions);
    freeArray(answers);

    return 0;
}