CC = gcc
CFLAG = -c -Wall


all: master slave

master: master.o
	$(CC) $^ -o $@.out -lrt
master.o: master.c
	$(CC) $(CFLAG) $^ -o $@

slave: slave.o
	$(CC) $^ -o $@.out -lm
slave.o: slave.c
	$(CC) $(CFLAG) $^ -o $@

clean:
	rm -f *.a *.o *.out data
