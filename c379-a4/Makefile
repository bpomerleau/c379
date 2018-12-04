CC=gcc
CFLAGS=-Wall -pedantic -ansi -std=c99 -pthread
LDFLAGS=-pthread
BIN=bin
SRC=src
OBJECTS=main.o io_cntrlr.o monitor.o task.o mutex.o

all: a4tasks

a4tasks: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o a4tasks

main.o: main.c main.h constants.h
	$(CC) $(CFLAGS) -c main.c

task.o: task.c task.h constants.h
	$(CC) $(CFLAGS) -c task.c

monitor.o: monitor.c monitor.h constants.h
	$(CC) $(CFLAGS) -c monitor.c

io_cntrlr.o: io_cntrlr.c io_cntrlr.h constants.h
	$(CC) $(CFLAGS) -c io_cntrlr.c

mutex.o: mutex.c mutex.h constants.h
	$(CC) $(CFLAGS) -c mutex.c

test: tests.c
	$(CC) $(CFLAGS) tests.c -o test

clean:
	rm -f a4tasks *.o *.tar

tar:
	rm -f submit.tar
	tar -cf submit.tar *.c *.h Makefile *.pdf
