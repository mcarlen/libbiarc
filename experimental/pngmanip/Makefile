CXXFLAGS=-g -O2 -Wall
INCLUDE=-I. -I${LIBBIARC}/include 
LIB_INC=-L. -L${LIBBIARC}/lib
LIB=-lbiarc -lpng -lm -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}

LINKER=g++
CXX=g++
GCC=gcc

.SUFFIXES: .cpp,.c

PROGS=plot plot4 gradienttest

all: ${PROGS}

.cpp.o:
	@echo Compile $@
	$(CXX) $(INCLUDE) ${CXXFLAGS} -c $<

.c.o:
	@echo Compile $@
	@$(GCC) -c $<

%.cpp: %.h
	touch $@

writhe: writhe.cpp
	@echo Build $@
	$(CXX) $(FLAGS) -c pngwrite.c
	$(CXX) $(FLAGS) -c writhe.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) pngwrite.o \
	writhe.o $(LIB)

plot: plot.o plot_funcs.o
	@echo Build $@
	$(CXX) $(FLAGS) -c pngwrite.c
	$(CXX) $(FLAGS) -c plot.cpp
	$(CXX) $(FLAGS) -c plot_funcs.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) pngwrite.o \
	plot.o plot_funcs.o $(LIB)

plot4: plot.cpp plot_funcs.o
	@echo Build $@
	$(CXX) $(FLAGS) -DCOMPUTE_IN_R4 -c pngwrite.c
	$(CXX) $(FLAGS) -DCOMPUTE_IN_R4 -c plot.cpp
	$(CXX) $(FLAGS) -DCOMPUTE_IN_R4 -c plot_funcs.cpp
	$(LINKER) $(FLAGS) -DCOMPUTE_IN_R4 -o $@ $(LIB_INC) pngwrite.o \
	plot.o plot_funcs.o $(LIB)

gradienttest: gradienttest.o
	@echo Build $@
	$(GCC) -c pngwrite.c
	$(GCC) -o $@ pngwrite.o gradienttest.o -lm -lpng

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f *.o
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf *.o *.png ii_files
