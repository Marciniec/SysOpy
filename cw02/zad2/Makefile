CC = gcc
CFLAG = -c -Wall

all: main main2

main: main.o
			$(CC) $^ -o $@.out

main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@

main2: main2.o
			$(CC) $^ -o $@.out

main2.o: main2.c
			$(CC)  $(CFLAG) $^ -o $@

clean:
			rm -f *.a *.o *.out
