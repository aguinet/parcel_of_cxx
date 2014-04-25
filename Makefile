CXX=g++
CC=g++
CXXFLAGS=-O2 -g -std=c++11 -Wall -Wextra
LDFLAGS=

all:bench

clean:
	$(RM) *.o
	$(RM) bench
