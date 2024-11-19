/* Write a multiple concurrent process program that does the following
   1. Displays "ascii modern art" composed of a series of 
      randomly generated blocks of 2 or more repeating characters

   The program should perform this task in the following way:
   1. Create a semaphore set of size 2 -- use IPC_PRIVATE as your key
      a. Initialize the semaphore representing the child to available
      b. Initialize the semaphore representing the parent to in use
   2. Create a segment of shared memory -- use IPC_PRIVATE as your key

   3. Create a child process that:
      a. Attaches the segment of shared memory
      b. Seeds the random number generator 
      c. Reserves the child semaphore
      d. Generates and stores the total number of blocks to generate. 
         This should be between 10 and 20
      c. For each block, generate and store the following values:
         1. The length of the block (i.e. the number of characters to display)
            This should be between 2 and 10
         2. The character that comprises the block.
            This should be between 'a' and 'z'
      d. Release the parent semaphore
      e. Reserve the child semaphore
      f. Detach the shared memory segment
      g. Release the parent semaphore

   4. Create a parent process that:
      a. Attaches the segment of shared memory
      b. Seeds the random number generator
      c. Reserve the parent semaphore 
      d. Generate a random width for the ASCII art. 
         This should be between 10 and 15
      e. Using the data stored in the shared memory segment, output an image
         Use the following tips: 
         1. One value stored in the segment should represent the number
            of (length, character) pairings. For each (length, character)
            pairing, you should output length instances of the given 
            character. This means if the pairing was (3, 'b'), you would
            output "bbb".
         2. The random image has basically been encoded use run-length
            encoding (RLE); RLE doesn't including the location of new lines.
            The location of new lines is determined by the random width 
            generated in step d. Each time you output width total characters,
            output a new line.
      f. Release the child semaphore
      g. Reserve the parent semaphore
      h. Detach the shared memory segment

   5. Delete the semaphore
   6. Delete the shared memory
   
   -- You can use/should use the binary semaphore protocol introduced in class
 */

/* Problem 3 -- List the inlcude files you need for this program. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "semun.h"
#include "binary_sem.h"

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) //Semaphore/shared memory perms
#define SEM_CHILD 0 //Child id
#define SEM_PARENT 1 //Parent id

int checkError(int e, const char *str) {
  if (e == -1)
    {
      if (e == EINTR) return -1;
      perror(str);
      exit(EXIT_FAILURE);
    }
  return e;
}

int genRand(int low, int high) {
  int rng = high - low + 1;
  double drnd = rand();
  int irnd = drnd / ((double) RAND_MAX + 1) * rng;
  return low + irnd;
}

enum colors {
   Black = 30,
   Red = 31,
   Green = 32,
   Yellow = 33,
   Blue = 34,
   Magenta = 35,
   Cyan = 36,
   White = 37,
   Reset = 0
};

//function to get a random int value from our colors enum
int getRandomColor() {
   int colors[] = {Black, Red, Green, Yellow, Blue, Magenta, Cyan, White};
   int numColors = sizeof(colors) / sizeof(colors[0]);
   return colors[rand() % numColors];
}

/* Problem 4 -- remember to declare a structure that represents the data
   stored in the shared memory */
struct shmseg {
   int charBlocks;  //How many blocks of chars we will generate
   int lengths[20];  //Lengths of each block
   char chars[20];   //Characters of each block
};

/* Problem 5 -- create a function to handle the code for the child. 
   Be certain to pass this function the id values for both the semaphore 
   and the shared memory segment */
void child(int semid, int shmid) {

   //Our shared memory
   struct shmseg *sharedMem;

   srand(time(NULL) + 1); //RNG Seed

   //Attach our memory
   sharedMem = shmat(shmid, NULL, 0);

   //Custom check error since we don't have a handler for a void pointer
   if (sharedMem == (void *)-1) {
        perror("shmat in child");
        exit(EXIT_FAILURE);
   }

   //Lock child semaphore
   reserveSem(semid, SEM_CHILD);

   // Generate charBlocks
   sharedMem->charBlocks = genRand(10, 20);

   //Generate each block
   for (int i = 0; i < sharedMem->charBlocks; i++) {
      sharedMem->lengths[i] = genRand(2, 10); //Block length
      sharedMem->chars[i] = genRand('a', 'z'); //Block character
   }

   releaseSem(semid, SEM_PARENT); //Unlock parent
   reserveSem(semid, SEM_CHILD); //Lock child

   //Detach shared memory
   checkError(shmdt(sharedMem), "shmdt in child");

   //Unlock parent sem
   releaseSem(semid, SEM_PARENT);

   exit(EXIT_SUCCESS);
}


/* Problem 6 -- create a function to handle the code for the parent. 
   Be certain to pass this function the id values for both the semaphore 
   and the shared memory segment */
void parent(int semid, int shmid) {

   //Our shared memory
   struct shmseg *sharedMem;

   srand(time(NULL) + 1); //RNG Seed

   //Attach our memory
   sharedMem = shmat(shmid, NULL, 0);

   //Custom check error since we don't have a handler for a void pointer
   if (sharedMem == (void *)-1) {
      perror("shmat in parent");
     exit(EXIT_FAILURE);
   }

   //Lock parent semaphore
   reserveSem(semid, SEM_PARENT);

   //How many of the same characters our parent will produce
   int width = genRand(10, 15);

   printf("Width: %d\n", width);
   int charCount = 0;

   //Loop thru our charBlocks array
   for (int i = 0; i < sharedMem->charBlocks; i++) {

      //If you want to have each character display a different color
      //Move this line of code to the loop below
      int color = getRandomColor(); //Gets random color

      //Loop thru our lengths for each block
      for (int j = 0; j < sharedMem->lengths[i]; j++) {
         printf("\033[0;%dm%c", color, sharedMem->chars[i]);
         charCount++;

         //End with new line if we reached our width
         if (charCount % width == 0)
            printf("\n");

      }
    }

   //End with new line 
   if (charCount % width != 0)
      printf("\n");

   releaseSem(semid, SEM_CHILD); //Unlock child
   reserveSem(semid, SEM_PARENT); //Lock Parent

   //Detach and remove shared memory and semaphores
   checkError(shmdt(sharedMem), "shmdt in parent");
   checkError(semctl(semid, 0, IPC_RMID), "remove semaphores");
   checkError(shmctl(shmid, IPC_RMID, NULL), "remove shared memory");

   exit(EXIT_SUCCESS);
}

/* Problem 7 -- implement function main */
int main(int argc, char *argv[]) {

   pid_t pid;
   int semid, shmid;
   key_t semK, shmK;

   //Generate keys
   shmK = ftok(".", 2000);
   semK = ftok(".", 4500);

   //Create a semaphore
   semid = checkError(semget(semK, 2, IPC_CREAT | OBJ_PERMS), "semget");

   checkError(semctl(semid, SEM_CHILD, SETVAL, 1), "init child semaphore"); //Child
   checkError(semctl(semid, SEM_PARENT, SETVAL, 0), "init parent semaphore"); //Parent

   //Create shared memory
   shmid = checkError(shmget(shmK, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS), "shmget");
   
   pid = fork(); //Create child process

   //Create shared memory
   checkError(pid, "Failed to create child");

   if(pid == 0)
      child(semid, shmid); //Parent function
   else
      parent(semid, shmid); //Child function

   return 0;
}