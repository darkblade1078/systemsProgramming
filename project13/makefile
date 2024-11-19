OBJS	= main.o binary_sem.o
SOURCE	= main.c binary_sem.c
HEADER	= semun.h binary_sem.h
OUT	= main
FLAGS	= -g -c -Wall
LFLAGS	= 
CC	= gcc

all:	main.out

main.out: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

binary_sem.o: binary_sem.c
	$(CC) $(FLAGS) binary_sem.c 

clean:
	rm -f $(OBJS) $(OUT)
