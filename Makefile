CXX=g++
CC=g++
CXXFLAGS=-O3 -g -std=c++11 -Wall -Wextra -fopenmp -march=native -mtune=native -ftree-vectorizer-verbose=1
LDFLAGS=-ltbb -ltbbmalloc -fopenmp

bench: bench.o

clean:
	rm *.o
	rm bench
