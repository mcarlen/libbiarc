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
	@$(CXX) $(INCLUDE) ${CXXFLAGS} -c $<

.c.o:
	@echo Compile $@
	@$(GCC) -c $<

writhe: writhe.cpp
	@echo Build $@
	$(CXX) $(FLAGS) -c pngwrite.c
	$(CXX) $(FLAGS) -c writhe.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) pngwrite.o \
	writhe.o $(LIB)

plot: plot.cpp
	@echo Build $@
	$(CXX) $(FLAGS) -c pngwrite.c
	$(CXX) $(FLAGS) -c plot.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) pngwrite.o \
	plot.o $(LIB)

plot4: plot.cpp
	@echo Build $@
	$(CXX) $(FLAGS) -DCOMPUTE_IN_R4 -c pngwrite.c
	$(CXX) $(FLAGS) -DCOMPUTE_IN_R4 -c plot.cpp
	$(LINKER) $(FLAGS) -DCOMPUTE_IN_R4 -o $@ $(LIB_INC) pngwrite.o \
	plot.o $(LIB)


ptplot: ptplot.o
	@echo Build $@
	$(CXX) $(FLAGS) -c pngwrite.c
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) pngwrite.o \
	ptplot.o $(LIB)

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
