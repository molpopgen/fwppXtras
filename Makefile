CXX=g++
CXXFLAGS=-std=c++11 -I. -O3 -Wall -W
LIBS=-lboost_system -lsequence -lgsl -lgslcblas -lz

all: manydemes1.o
	$(CXX) -o manydemes1 manydemes1.o $(LIBS)

clean:
	rm -f *.o
	find . -perm +111 -type f | xargs rm -f

manydemes1.o: common_ind.hpp
