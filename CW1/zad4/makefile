CC = gcc
CFLAG = -c -Wall

all: main3
main3: main3.o
				$(CC) $< -g -L../zad1 -lcontactlibstatic -o $@.out

main3.o: main3.c
				$(CC) $(CFLAG) -o $@ $<

clean:
				rm -f *.a *.o *.so *.out
