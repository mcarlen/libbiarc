include ../Makefile.config

INCLUDE+=-I../include

ifeq ($(RENDERMAN),YES)
LIB=libbiarc_pixie.so
STATIC_LIB=libbiarc_pixie.a
else
LIB=libbiarc.so
STATIC_LIB=libbiarc.a
endif

SRC=PKFmanip.cpp Vector3.cpp Vector4.cpp Curve.cpp CurveBundle.cpp \
	Biarc.cpp Matrix3.cpp Matrix4.cpp Tube.cpp TubeBundle.cpp
OBJ=${SRC:.cpp=.o}

#####  RULES  ####
.SUFFIXES: .cpp

.cpp.o:
	@echo Compile $<
	@$(CXX) $(INCLUDE) ${CXXFLAGS} -fPIC -c $<

all: $(LIB) $(STATIC_LIB)

$(LIB): $(OBJ)
	@echo Build shared lib $(LIB)
	@$(LINKER) -shared $(OBJ) -o $(LIB)

$(STATIC_LIB): $(OBJ)
	@echo using $?
	@echo Build static lib $(STATIC_LIB)
	@ar crs $(STATIC_LIB) $? 

###--------------###
###    Clean     ###
###--------------###

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f $(OBJ)
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}` ${LIB} and ${STATIC_LIB}
	@rm -f $(OBJ) $(LIB) $(STATIC_LIB)
	@rm -f *~ *% .*~ .*% core
	@rm -rf ii_file
