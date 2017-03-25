CC = gcc
CFLAG = -c -Wall

all: staticmeasuretime1 sharedmeasuretime1 dynamicmeasuretime1 staticmeasuretime2 sharedmeasuretime2 dynamicmeasuretime2 staticmeasuretime3 sharedmeasuretime3 dynamicmeasuretime3 staticmeasuretimef sharedmeasuretimef dynamicmeasuretimef staticmeasuretimes sharedmeasuretimes dynamicmeasuretimes

staticmeasuretime1: measuretime.o
				$(CC) $< -O1 -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretime1: measuretime.o
				$(CC) $< -O1 -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

dynamicmeasuretime1:  measuretime2.o
				$(CC) $< -O1 -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

staticmeasuretime2: measuretime.o
				$(CC) $< -O2 -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretime2: measuretime.o
				$(CC) $< -O2 -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

dynamicmeasuretime2:  measuretime2.o
				$(CC) $< -O2 -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out
staticmeasuretime3: measuretime.o
				$(CC) $< -O3 -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretime3: measuretime.o
				$(CC) $< -O3 -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

dynamicmeasuretime3:  measuretime2.o
				$(CC) $< -O3 -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out

staticmeasuretimef: measuretime.o
				$(CC) $< -Ofast -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretimef: measuretime.o
				$(CC) $< -Ofast -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out
dynamicmeasuretimef:  measuretime2.o
				$(CC) $< -Ofast -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out
staticmeasuretimes: measuretime.o
				$(CC) $< -Os -L../zad1 -lcontactlibstatic -o $@.out
				./$@.out
sharedmeasuretimes: measuretime.o
				$(CC) $< -Os -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out
dynamicmeasuretimes:  measuretime2.o
				$(CC) $< -Os -ldl -L../zad1 -Wl,-rpath=../zad1 -lcontactlibshare -o $@.out
				./$@.out


measuretime.o: ../zad2/main.c
				$(CC) $(CFLAG) -o $@ $<
measuretime2.o: ../zad3a/main2.c
				$(CC) $(CFLAG) -o $@ $<

clean:
				rm -f *.a *.o *.so *.out
