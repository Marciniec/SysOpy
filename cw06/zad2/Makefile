CC = gcc
CFLAG = -c -Wall
LDLIBS = -lrt
all: main client

main: main.o
			$(CC) $^ -o $@.out $(LDLIBS)


main.o: main.c
			$(CC)  $(CFLAG) $^ -o $@ $(LDLIBS)

client: client.o
			$(CC) $^ -o $@.out $(LDLIBS)

client.o: client.c
			$(CC)  $(CFLAG) $^ -o $@ $(LDLIBS)
clean:
			rm -f *.a *.o *.out
