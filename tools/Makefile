include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)
PROGS=$(SRC:.cpp=)

.PHONY: all

all: $(PROGS)

sigma_composition: sigma_composition.cpp
	$(LINKER) -o $@ $<

animate: animate.cpp
	soqt-config --build animate animate.cpp

contactset4: contactset.cpp
	@echo Build $@
	@$(LINKER) $(FLAGS) -DDim4 -o $@ $(LIB_INC) $< $(LIB)

follow_contact4: follow_contact.cpp
	@echo Build $@
	@$(LINKER) $(FLAGS) -DDim4 -o $@ $(LIB_INC) $< $(LIB)

%: %.cpp
	@echo Build $@
	@$(LINKER) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf ii_files
