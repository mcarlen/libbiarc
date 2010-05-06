include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc
FLAGS=${INCLUDE} ${CXXFLAGS} # -DTEST
SRC=$(wildcard *.cpp)

PROF=${FLAGS} -O2

all: anneal anneal4 sono curvatureflow

anneal: anneal.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

anneal4: anneal4.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

curvatureflow: curvatureflow.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

sono: sono.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

open_sono: open_sono.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

sono_op: sono_optimized.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

clean realclean:
	@-rm -f anneal anneal4 sono curvatureflow *.o
