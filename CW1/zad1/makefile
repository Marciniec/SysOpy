CC = gcc
CFLAG = -c -Wall

all: libcontactlibstatic libcontactlibshare

libcontactlibstatic: contactlib.c
									$(CC)  $(CFLAG) $^
									ar -cr $@.a contactlib.o

libcontactlibshare: contactlib.c
									$(CC) -fPIC -c $^
									$(CC)  -shared -fPIC -o $@.so contactlib.o


clean:
				rm -f *.a *.o *.so
