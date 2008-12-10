CXXFLAGS=-g -O2 -Wall
INCLUDE=-I. -I${LIBBIARC}/include 
LIB_INC=-L. -L${LIBBIARC}/lib
LIB=-lbiarc -lpng -lm
FLAGS=${INCLUDE} ${CXXFLAGS}

LINKER=g++
CXX=g++
GCC=gcc

.SUFFIXES: .cpp

PROGS=valleyflow

all: ${PROGS}

.cpp.o:
	@echo Compile $@
	@$(CXX) $(INCLUDE) ${CXXFLAGS} -c $<

.c.o:
	@echo Compile $@
	@$(GCC) -c $<

valleyflow: valleyflow.o ../pngmanip/pngwrite.o
	@echo Build $@
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) ../pngmanip/pngwrite.o \
	valleyflow.o $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f *.o
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf *.o *.png ii_files
