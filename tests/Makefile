include ../Makefile.config

INCLUDE=-I. -I../include 
LIB_INC=-L. -L../lib
LIB= -lbiarc
# LIB= ../lib/libbiarc.a      # static linking
FLAGS=${INCLUDE} ${CXXFLAGS}

.PHONY: test basic

all: verb test basic

TESTS=SimpleTest.h BasicTest.h

verb:
	@echo "Run tests"

library:
	cd ../lib; make

basic: basic.cpp library
	g++ ${INCLUDE} -o basic ${LIB_INC} ${LIB} basic.cpp
	 LD_LIBRARY_PATH=../lib/ ./basic

test: test.cpp library
	g++ ${INCLUDE} -o test ${LIB_INC} ${LIB} test.cpp
	LD_LIBRARY_PATH=../lib/ ./test

basic.cpp: BasicTest.h
	@cxxtestgen.pl --error-printer -o $@ $^

test.cpp: SimpleTest.h
	@cxxtestgen.pl --error-printer -o $@ $^

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files
	@-rm -f test.cpp basic.cpp test basic

realclean : clean
	@echo Purge `basename ${PWD}`
	@-rm -f *~ *% .*~ .*% core *.pkf ii_files
