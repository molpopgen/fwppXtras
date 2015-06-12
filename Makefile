CXX=g++
#CXXFLAGS=-std=c++11 -I/home/kevin/src/fwdpp -I. -O2 -pg -Wall -W
CXXFLAGS=-std=c++11 -I. -O2 -pg -Wall -W
LIBS=-lboost_system -lsequence -lgsl -lgslcblas -lz

all: manydemes1.o fewlargedemes1.o
	$(CXX) $(CXXFLAGS) -o manydemes1 manydemes1.o $(LIBS)
	$(CXX) $(CXXFLAGS) -o fewlargedemes1 fewlargedemes1.o $(LIBS)

clean:
	rm -f *.o
	find . -perm +111 -type f | xargs rm -f

manydemes1.o: common_ind.hpp
fewlargedemes1.o: common_ind.hpp
