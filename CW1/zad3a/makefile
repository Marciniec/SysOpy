CC = gcc
CFLAG = -c -Wall

all: staticmeasuretime sharedmeasuretime dynamicmeasuretime

staticmeasuretime: measuretime.o
				$(CC) $< -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretime: measuretime.o
				$(CC) $< -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

dynamicmeasuretime:  measuretime2.o
				$(CC) $< -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out
measuretime.o: ../zad2/main.c
				$(CC) $(CFLAG) -o $@ $<
measuretime2.o: main2.c
				$(CC) $(CFLAG) -o $@ $<

clean:
				rm -f *.a *.o *.so *.out
