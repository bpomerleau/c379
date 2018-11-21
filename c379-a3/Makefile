CC=gcc
CFLAGS=-Wall -pedantic -ansi -std=c99
BIN=bin
SRC=src

all: a3sdn

a3sdn: main.o controller.o switch.o fifo_cntrl.o packet.o socket_cntrl.o
	$(CC) main.o controller.o switch.o fifo_cntrl.o socket_cntrl.o packet.o -o a3sdn

main.o: main.c main.h constants.h
	$(CC) $(CFLAGS) -c main.c

controller.o: controller.c controller.h fifo_cntrl.h packet.h socket_cntrl.h constants.h
	$(CC) $(CFLAGS) -c controller.c

switch.o: switch.c switch.h fifo_cntrl.h packet.h socket_cntrl.h constants.h
	$(CC) $(CFLAGS) -c switch.c

fifo_cntrl.o: fifo_cntrl.c fifo_cntrl.h constants.h
	$(CC) $(CFLAGS) -c fifo_cntrl.c

socket_cntrl.o: socket_cntrl.c socket_cntrl.h constants.h
	$(CC) $(CFLAGS) -c socket_cntrl.c

packet.o: packet.c packet.h constants.h
	$(CC) $(CFLAGS) -c packet.c

test: tests.c
	$(CC) $(CFLAGS) tests.c -o test

clean:
	rm -f a3sdn *.o *.tar

tar:
	rm -f submit.tar
	tar -cf submit.tar *.c *.h Makefile *.pdf
