CC = gcc
CFLAG = -c -Wall

all: main loop

main: main.o
			$(CC) $^ -o $@.out


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@
loop: loop.o
			$(CC) $^ -o $@.out
loop.o: loop.c
			$(CC)  $(CFLAG) $^ -o $@


clean:
			rm -f *.a *.o *.out
