CC = gcc
CFLAG = -c -Wall

all: main client

main: main.o
			$(CC) $^ -o $@.out


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@

client: client.o
			$(CC) $^ -o $@.out

client.o: client.c
			$(CC)  $(CFLAG) $^ -o $@
clean:
			rm -f *.a *.o *.out
