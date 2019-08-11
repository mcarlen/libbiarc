include ../Makefile.config

INCLUDE+=-I. -I../include
LIB_INC+=-L. -L../lib
LIB+=-lbiarc
FLAGS=${INCLUDE} ${CXXFLAGS}
SRC=$(wildcard *.cpp)
PROGS=$(SRC:.cpp=)

.PHONY: all

all: $(PROGS)

billiard_poly: billiard_poly.cpp
	$(CXX) -o $@ $<

pkf2rib: pkf2rib.cpp
ifeq ($(RENDERMAN),YES)
	$(CXX) -DPRECISION_HACK -DRENDERMAN -I../include -L../lib -L${PIXIEHOME}/lib -I${PIXIEHOME}/include pkf2rib.cpp -o pkf2rib -lbiarc_pixie -lri
else
	@echo "No Pixie support compiled in. Ignore."
endif

animate: animate.cpp
	soqt-config --build animate animate.cpp

contactset4: contactset.cpp
	@echo Build $@
	@$(LD) $(FLAGS) -DDim4 -o $@ $(LIB_INC) $< $(LIB)

follow_contact4: follow_contact.cpp
	@echo Build $@
	@$(LD) $(FLAGS) -DDim4 -o $@ $(LIB_INC) $< $(LIB)

%: %.cpp
	@echo Build $@
	@$(LD) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf ii_files
