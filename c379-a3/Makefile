CC=gcc
CFLAGS=-Wall -pedantic -ansi -std=c99
BIN=bin
SRC=src

all: a3sdn

a3sdn: main.o controller.o switch.o io_cntrl.o packet.o
	$(CC) main.o controller.o switch.o io_cntrl.o  packet.o -o a3sdn

main.o: main.c main.h constants.h
	$(CC) $(CFLAGS) -c main.c

controller.o: controller.c controller.h io_cntrl.h packet.h constants.h
	$(CC) $(CFLAGS) -c controller.c

switch.o: switch.c switch.h io_cntrl.h packet.h constants.h
	$(CC) $(CFLAGS) -c switch.c

io_cntrl.o: io_cntrl.c io_cntrl.h constants.h
	$(CC) $(CFLAGS) -c io_cntrl.c

packet.o: packet.c packet.h constants.h
	$(CC) $(CFLAGS) -c packet.c

test: tests.c
	$(CC) $(CFLAGS) tests.c -o test

clean:
	rm -f a3sdn *.o *.tar

tar:
	rm -f submit.tar
	tar -cf submit.tar *.c *.h Makefile *.pdf
