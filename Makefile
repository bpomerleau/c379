CC=gcc
CFLAGS=-Wall -pedantic -ansi -std=c99
BIN=bin
SRC=src

all: a2sdn

a2sdn: main.o controller.o switch.o fifo_cntrl.o
	$(CC) main.o controller.o switch.o fifo_cntrl.o -o a2sdn

main.o: main.c main.h constants.h
	$(CC) $(CFLAGS) -c main.c

controller.o: controller.c controller.h constants.h fifo_cntrl.h
	$(CC) $(CFLAGS) -c controller.c

switch.o: switch.c switch.h constants.h fifo_cntrl.h
	$(CC) $(CFLAGS) -c switch.c

fifo_cntrl.o: fifo_cntrl.c fifo_cntrl.h constants.h
	$(CC) $(CFLAGS) -c fifo_cntrl.c

test: tests.o
	$(CC) tests.o -o test

test.o: tests.c
	$(CC) $(CFLAGS) -c tests.c -o test.o

clean:
	rm -f a2sdn *.o *.tar

tar:
	rm -f submit.tar
	tar -cf submit.tar *.c *.h Makefile *.pdf
