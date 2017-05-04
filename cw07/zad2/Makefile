CC = gcc
CFLAG = -c -Wall
LDLIBS = -lrt
all: sleepingbarber client

sleepingbarber: sleepingbarber.o
			$(CC) $^ -o $@.out $(LDLIBS) -pthread -g


sleepingbarber.o: sleepingbarber.c
			$(CC)  $(CFLAG) $^ -o $@ $(LDLIBS)

client: client.o
			$(CC) $^ -o $@.out $(LDLIBS) -pthread

client.o: client.c
			$(CC)  $(CFLAG) $^ -o $@ $(LDLIBS)
clean:
			rm -f *.a *.o *.out
