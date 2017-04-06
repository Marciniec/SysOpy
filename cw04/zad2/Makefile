CC = gcc
CFLAG = -c -Wall

all: main

main: main.o
			$(CC) $^ -o $@.out


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@


clean:
			rm -f *.a *.o *.out

