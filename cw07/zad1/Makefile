CC = gcc
CFLAG = -c -Wall

all: sleepingBarber client

sleepingBarber: sleepingBarber.o
			$(CC) $^ -o $@.out


sleepingBarber.o: sleepingBarber.c
			$(CC)  $(CFLAG) $^ -o $@

client: client.o
			$(CC) $^ -o $@.out

client.o: client.c
			$(CC)  $(CFLAG) $^ -o $@
clean:
			rm -f *.a *.o *.out
