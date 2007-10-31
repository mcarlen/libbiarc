include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)

PROF=-O2

all: anneal

anneal: anneal.cpp
	$(LINKER) ${PROF} -I../include -o $@ -L../lib $^ ../lib/libbiarc.a -lrt

clean realclean:
	@-rm -f anneal
