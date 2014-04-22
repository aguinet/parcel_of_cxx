CXX=g++
CC=g++
CXXFLAGS=-O3 -g -std=c++11 -Wall -Wextra -fopenmp
LDFLAGS=-ltbb -ltbbmalloc -fopenmp

bench: bench.o
