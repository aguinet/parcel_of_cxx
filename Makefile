CXX=g++
CC=g++
CXXFLAGS=-O3 -g -std=c++11 -Wall -Wextra -fopenmp -march=native -mtune=native -I/usr/include/python2.7 -fPIC
LDFLAGS=-ltbb -ltbbmalloc -fopenmp -lboost_python -lpython2.7

all: bench pymap.so

bench: bench.o
pymap.so: pymap.o
	g++ $(LDFLAGS) pymap.o -shared -o pymap.so

clean:
	rm *.o
	rm bench
