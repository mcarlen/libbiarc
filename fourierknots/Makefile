include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc
FLAGS=${INCLUDE} ${CXXFLAGS}
PROGS=$(SRC:.cpp=)

all: coeff2pkf

.PHONY: $(PROGS)

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(FLAGS) -o $@ -c $<


coeff2pkf: coeff2pkf.o fourier_syn.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) coeff2pkf.o fourier_syn.o $(LIB)

$(PROGS):
	@echo Build $@
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf ii_files
