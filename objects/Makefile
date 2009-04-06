include ../Makefile.config

.PHONY: all

SRC=$(wildcard *.cpp)
PROGS=$(SRC:.cpp=)
INCLUDE =-I. -I../include 
LIB_INC=-L. -L../lib
LIB=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}

all: $(PROGS)

$(PROGS):
	@echo Build $@
	@$(LINKER) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

pkf: $(PROGS)
	@echo Make pkf files
	@./line 1.0 9
	@./circle 1.0 12
	@./sinus 1.0 1.0 6.28 40
	@./helix 1.0 1.0 .5 12.5 100
	@./spiral 1.0 1.0 1.0 30 200
	@./solenoid 0.2 1.0 8 200
	@./crouzy 5 5 .4 .08 2 100 2000
	@./borromean 80
	@./inf 3 1 .1 100


clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -rf ${PROGS} *~ *% .*~ .*% core *.pkf ii_files
