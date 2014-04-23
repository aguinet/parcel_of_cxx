CXX=g++
CC=g++
CXXFLAGS=-O2 -g -std=c++11 -Wall -Wextra
LDFLAGS=

bench: bench.o

clean:
	rm *.o
	rm bench
