include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)

PROF=${FLAGS} -O2

all: anneal anneal4

anneal: anneal.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

anneal4: anneal4.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

curvatureflow: curvatureflow.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

clean realclean:
	@-rm -f anneal anneal4
