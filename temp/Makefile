PROGS = curvature_view torsion_view

include ../Makefile.config

INCLUDE  = -I$(COIN)/include -I$(SOQT)/include -D_REENTRANT -I/usr/X11R6/include -I$(QT_INC)
INC_LIB = -L../lib -L$(COIN)/lib -L$(SOQT)/lib -L/usr/local/lib -L/usr/X11R6/lib -L$(QT_LIB)

######### LIBRARIES  #########################

LIBS = `soqt-config --libs` -lbiarc

######### FLAGS      #########################

FLAGS = $(INCLUDE) `soqt-config --cxxflags` ${CXXFLAGS} # -D_USE_QT_  -D__debug__

######### SOURCE FILES #######################

SRC = curvature_view.cpp torsion_view.cpp 
OBJ = $(SRC:.cpp=.o)

#####  RULES  ####
.SUFFIXES: .cpp 

.cpp.o:
	@echo Compile $<
	@$(CXX) ${FLAGS} -c $<

################ COMPILATION #####################

all: $(PROGS)

curvature_view: curvature_view.o
	@echo Link ${<:.o=}
	@${LINKER} ${FLAGS} -o curvature_view $(INC_LIB) curvature_view.o $(LIBS)

torsion_view: torsion_view.o
	@echo Link ${<:.o=}
	@${LINKER} ${FLAGS} -o torsion_view $(INC_LIB) torsion_view.o $(LIBS)

###--------------###
###    Clean     ###
###--------------###

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f $(OBJ)
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f $(OBJ) $(PROGS)
	@-rm -f *~ *% .*~ .*% core
	@-rm -rf ii_files
