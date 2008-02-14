include ../Makefile.config

INCLUDE=-I. -I../include 
LIB_INC=-L. -L../lib
LIB= -lbiarc
# LIB= ../lib/libbiarc.a      # static linking
FLAGS=${INCLUDE} ${CXXFLAGS}

.PHONY: test runthem

all: test

TESTS=SimpleTest.h BasicTest.h ThicknessTest.h
TESTSCPP=${TESTS:.h=.cpp}
BUILDTESTS=${TESTS:.h=}

test: library verb runthem

verb:
	@echo "Run tests"

library:
	cd ../lib; make

runthem: $(BUILDTESTS)
	@for i in $^; do \
	  echo "Run test $$i"; LD_LIBRARY_PATH=../lib/ ./$$i; \
	done

%: %.cpp
	g++ ${INCLUDE} -o $@ ${LIB_INC} ${LIB} $<

%.cpp: %.h
	@cxxtestgen.pl --error-printer -o $@ $^

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files
	@-rm -f ${TESTSCPP}

realclean : clean
	@echo Purge `basename ${PWD}`
	@-rm -f ${BUILDTESTS}
	@-rm -f *~ *% .*~ .*% core *.pkf ii_files
