all: a1jobs a1mon

clean:
	rm a1jobs a1mon
a1jobs: a1jobs.c
	gcc a1jobs.c -o a1jobs

a1mon: a1mon.c
	gcc a1mon.c -o a1mon

test: tests.cpp
	gcc tests.c -o test
