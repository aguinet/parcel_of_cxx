CXX=g++
CC=g++
CXXFLAGS=-O2 -g -std=c++11 -Wall -Wextra -fopenmp
LDFLAGS=-ltbb -ltbbmalloc -fopenmp

bench: bench.o

clean:
	rm *.o
	rm bench
