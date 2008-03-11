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

anneal4: anneal.cpp
	$(LINKER) ${PROF} -DCOMPUTE_IN_R4 -o $@ ${LIB_INC} $^ ${LIB}

curvatureflow: curvatureflow.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

clean realclean:
	@-rm -f anneal anneal4
