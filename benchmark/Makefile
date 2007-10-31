include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)

PROF=-O2
#-g -pg
KNOTLIB=${HOME}/work/links-knots

all: bench benchben

bench: bench.cpp
	$(LINKER) ${PROF} -I../include -o $@ -L../lib bench.cpp ../lib/libbiarc.a -lrt

benchben: bench.cpp
	$(LINKER) ${PROF} -DBENCH_BEN -DLINUX -I${KNOTLIB}/include -o $@ bench.cpp ${KNOTLIB}/lib/libknot.a -lrt

clean:
	@-rm -f bench benchben

realclean: clean
