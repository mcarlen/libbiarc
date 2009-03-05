include ../Makefile.config

INCLUDE+=-I. -I../include 
LIB_INC+=-L. -L../lib
LIB+=-lbiarc -lrt
FLAGS=${INCLUDE} ${CXXFLAGS}
PROGS=$(SRC:.cpp=)

all: coeff2pkf

.PHONY: $(PROGS)

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(FLAGS) -o $@ -c $<


new_fourier_anneal: new_fourier_anneal.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o ../experimental/annealing/my_anneal.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) new_fourier_anneal.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o $(LIB)

new_fourier_anneal_fast: new_fourier_anneal_fast.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o ../experimental/annealing/my_anneal.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) new_fourier_anneal_fast.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o $(LIB) -lrt

tref_fourier_anneal_on_S3: tref_fourier_anneal_on_S3.o new_fourier_anneal.cpp fourier_syn.o fourier_3_1.o ../experimental/annealing/my_anneal.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) tref_fourier_anneal_on_S3.o fourier_syn.o fourier_3_1.o $(LIB)

fourier_anneal_on_S3: fourier_anneal_on_S3.o new_fourier_anneal.cpp fourier_syn.o fourier_3_1.o ../experimental/annealing/my_anneal.cpp
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) fourier_anneal_on_S3.o fourier_syn.o fourier_3_1.o $(LIB)


profile: clean
	$(CXX) $(FLAGS) -DPROFILING -o profile $(LIB_INC) fourier_anneal.cpp fourier_syn.cpp fourier_3_1.cpp -pg $(LIB)
	./profile
	gprof -b profile

#fourier_anneal_generic: fourier_anneal_generic.o fourier_syn.o
#	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) fourier_anneal_generic.o fourier_syn.o $(LIB)

#fourier_anneal_41: fourier_anneal_41.o fourier_syn.o fourier_4_1.o
#	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) fourier_anneal_41.o fourier_4_1.o fourier_syn.o $(LIB)


coeff2pkf: coeff2pkf.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) coeff2pkf.o fourier_3_1.o fourier_4_1.o fourier_5_1.o fourier_syn.o $(LIB)

guess_coeffs: guess_coeffs.o fourier_syn.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) guess_coeffs.o fourier_syn.o $(LIB)

fcurvature: fcurvature.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) fcurvature.o fourier_syn.o fourier_3_1.o fourier_4_1.o fourier_5_1.o $(LIB)


test: test.o fourier_syn.o fourier_4_1.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) test.o fourier_syn.o fourier_4_1.o $(LIB)

sym: sym.o fourier_syn.o
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) sym.o fourier_syn.o $(LIB)

$(PROGS):
	@echo Build $@
	$(LINKER) $(FLAGS) -o $@ $(LIB_INC) $@.cpp $(LIB)

clean :
	@echo Cleaning `basename ${PWD}`
	-rm -f *.o
	@-rm -rf ii_files

realclean : clean
	@echo Purge `basename ${PWD}`
	@-rm -f $(PROGS) *~ *% .*~ .*% core *.pkf ii_files
