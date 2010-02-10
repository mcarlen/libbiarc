include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)

PROF=${FLAGS} -O2

all: gradientflow gradientflow4

gradientflow: gradientflow.cpp
	$(LINKER) ${PROF} -o $@ ${LIB_INC} $^ ${LIB}

gradientflow4: gradientflow.cpp
	$(LINKER) ${PROF} -DCOMPUTE_IN_R4 -o $@ ${LIB_INC} $^ ${LIB}

clean realclean:
	@-rm -f gradientflow gradientflow4
