include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)
PROGS=$(SRC:.cpp=)

.PHONY: $(PROGS)

all: $(PROGS)
$(PROGS):
	@echo Build $@
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf ii_files
