CC=gcc
CFLAGS=-Wall -pedantic -ansi -std=c99
BIN=bin
SRC=src

all: a1jobs a1mon


clean:
	rm -f a1jobs a1mon *.o *.tar
a1jobs: a1jobs.c
	$(CC) a1jobs.c -o a1jobs

a1mon: a1mon.o ptree.o
	$(CC) a1mon.o ptree.o -o a1mon

a1mon.o: a1mon.c
	$(CC) $(CFLAGS) -c a1mon.c

ptree.o: ptree.c
	$(CC) $(CFLAGS) -c ptree.c

test: tests.o
	$(CC) tests.o -o test

test.o: tests.c
	$(CC) $(CFLAGS) -c tests.c -o test.o

tar:
	rm -f submit.tar
	tar -cf submit.tar *.c *.h Makefile *.pdf
