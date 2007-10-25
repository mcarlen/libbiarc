include ../Makefile.config

INCLUDE=-I. -I../include 
LIB_INC=-L. -L../lib
LIB=-lbiarc
FLAGS=${INCLUDE} ${CXXFLAGS}

.PHONY: test

all: $(PROGS) test

TESTS=SimpleTest.h BasicTest.h

basic: basic.cpp
	g++ ${INCLUDE} -o basic ${LIB_INC} ${LIB} $^
	./basic

test: test.cpp
	g++ ${INCLUDE} -o test ${LIB_INC} ${LIB} $^
	./test

basic.cpp: BasicTest.h
	cxxtestgen.pl --error-printer -o $@ $^

test.cpp: SimpleTest.h
	cxxtestgen.pl --error-printer -o $@ $^

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files
	@-rm -f test.cpp basic.cpp test basic

realclean : clean
	@echo Purge `basename ${PWD}`
	@-rm -f *~ *% .*~ .*% core *.pkf ii_files
