include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)
PROGS=$(SRC:.cpp=)

.PHONY: all

all: $(PROGS)

pkf2rib: pkf2rib.cpp
	g++ -DPRECISION_HACK -DRENDERMAN -I../include -L../lib -L${PIXIEHOME}/lib -I${PIXIEHOME}/include pkf2rib.cpp -o pkf2rib -lbiarc_pixie -lrt -lri

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
