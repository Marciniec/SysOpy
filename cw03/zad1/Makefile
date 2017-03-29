CC = gcc
CFLAG = -c -Wall

all: main dem

main: main.o
			$(CC) $^ -o $@.out


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@
dem: demonstration.o
			$(CC) $^ -o $@.out
demonstration.o: demonstration.c
			$(CC)  $(CFLAG) $^ -o $@


clean:
			rm -f *.a *.o *.out
