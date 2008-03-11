include ../Makefile.config

INCLUDE+=-I../include

ifeq ($(RENDERMAN),YES)
LIB_FLOAT=libbiarc_pixie.so
STATIC_LIB_FLOAT=libbiarc_pixie.a
TODO=${LIB_FLOAT} ${STATIC_LIB}
else
LIB_FLOAT=libbiarc_float.so
STATIC_LIB_FLOAT=libbiarc_float.a
LIB_DOUBLE=libbiarc.so
STATIC_LIB_DOUBLE=libbiarc.a
TODO=${LIB_FLOAT} ${STATIC_LIB_FLOAT} ${LIB_DOUBLE} ${STATIC_LIB_DOUBLE}
endif

SRC=$(wildcard *.cpp)
OBJ_FLOAT=${SRC:.cpp=_float.o}
OBJ_DOUBLE=${SRC:.cpp=_double.o}

#####  RULES  ####
.SUFFIXES: .cpp

%_float.o: %.cpp
	@echo Compile Float $<
	@$(CXX) $(INCLUDE) ${CXXFLAGS} -DPRECISION_HACK -fPIC -c $< -o $@

%_double.o: %.cpp
	@echo Compile Double $<
	@$(CXX) $(INCLUDE) ${CXXFLAGS} -fPIC -c $< -o $@

all: $(TODO)

# Build float only
float: ${LIB_FLOAT} ${STATIC_LIB_FLOAT}

# Build double only
double: ${LIB_DOUBLE} ${STATIC_LIB_DOUBLE}

$(LIB_FLOAT): ${OBJ_FLOAT}
	@echo Build shared lib $(LIB_FLOAT)
	@$(LINKER) -shared ${OBJ_FLOAT} -o $(LIB_FLOAT)

$(STATIC_LIB_FLOAT): ${OBJ_FLOAT}
	@echo Build static lib $(STATIC_LIB_FLOAT)
	@ar crs $(STATIC_LIB_FLOAT) ${OBJ_FLOAT}

$(LIB_DOUBLE): ${OBJ_DOUBLE}
	@echo Build shared lib $(LIB_DOUBLE)
	@$(LINKER) -shared ${OBJ_DOUBLE} -o $(LIB_DOUBLE)

$(STATIC_LIB_DOUBLE): ${OBJ_DOUBLE}
	@echo Build static lib $(STATIC_LIB_DOUBLE)
	@ar crs $(STATIC_LIB_DOUBLE) ${DOUBLE}


###--------------###
###    Clean     ###
###--------------###

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f $(OBJ_FLOAT) $(OBJ_DOUBLE)
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@rm -f $(OBJ_FLOAT) $(STATIC_LIB)
	@rm -f $(OBJ_DOUBLE) $(LIB_DOUBLE) $(STATIC_LIB_DOUBLE)
	@rm -f *~ *% .*~ .*% core
	@rm -rf ii_file
