include ../../Makefile.config

INCLUDE+=-I. -I../../include 
LIB_INC+=-L. -L../../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS} # -DTEST
SRC=$(wildcard *.cpp)

PROF=${FLAGS} -O2

all: anneal_param

anneal_param: anneal_param.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

clean realclean:
	@-rm -f anneal_param *.o
