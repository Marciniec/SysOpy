CC = gcc
CFLAG = -c -Wall
LDLIBS = -lpthread

all: main main2 main3

main: main.o
			$(CC) $^ -o $@.out $(LDLIBS) -g


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@

main2: main2.o
			$(CC) $^ -o $@.out $(LDLIBS) -g


main2.o: main2.c
			$(CC)  $(CFLAG) $^ -o $@

main3: main3.o
			$(CC) $^ -o $@.out $(LDLIBS) -g

main3.o: main3.c
			$(CC)  $(CFLAG) $^ -o $@
fill: fill.o
			$(CC) $^ -o $@.out

fill.o: fill.c
			$(CC)  $(CFLAG) $^ -o $@

clean:
			rm -f *.a *.o *.out
