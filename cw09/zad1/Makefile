CC = gcc
CFLAG = -c -Wall
LDLIBS = -lpthread

all: main

main: main.o
			$(CC) $^ -o $@.out $(LDLIBS) -g


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@

clean:
			rm -f *.a *.o *.out
