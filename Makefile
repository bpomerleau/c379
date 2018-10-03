all: a1jobs a1mon

clean:
	rm a1jobs a1mon
a1jobs: a1jobs.cpp
	gcc a1jobs.cpp -o a1jobs

a1mon: a1mon.cpp
	gcc a1mon.cpp -o a1mon

test: tests.cpp
	gcc tests.cpp -o test
