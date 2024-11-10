OBJS	= BST.o main.o
SOURCE	= BST.c main.c
HEADER	= BST.h
OUT	= test
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

BST.o: BST.c
	$(CC) $(FLAGS) BST.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 


clean:
	rm -f $(OBJS) $(OUT)
