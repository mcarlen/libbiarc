PROGS = kv curveview xyzview multiview skeleton skeleton_open merge_all main # kv4

include ../Makefile.config

######### INCLUDES ###########################

COIN=`coin-config --prefix`
SOQT=`soqt-config --prefix`
INCLUDE  = -I$(COIN)/include -I$(SOQT)/include -D_REENTRANT -I${QT_INC} -I/usr/X11R6/include
INC_LIB = -L../lib -L$(COIN)/lib -L$(SOQT)/lib -L/usr/lib -L/usr/local/lib -L${QT_LIB} -L/usr/X11R6/lib

######### LIBRARIES  #########################

LIBS = `soqt-config --libs` -lbiarc_float

######### FLAGS      #########################

FLAGS = $(INCLUDE) `soqt-config --cxxflags` $(CXXFLAGS) -DPRECISION_HACK # -D_USE_QT_  -D__debug__

######### SOURCE FILES #######################

SRC = curveview.cpp knotview.cpp SoKnot.cpp xyzview.cpp \
	multiview.cpp skeleton.cpp skeleton_open.cpp merge_all.cpp \
	main.cpp trefoil_builder.cpp
OBJ = $(SRC:.cpp=.o)

#####  RULES  ####
.SUFFIXES: .cpp 

.cpp.o:
	@echo Compile $<
	@$(CXX) ${FLAGS} -c $<

################ COMPILATION #####################

all: $(PROGS)

skeleton_open: skeleton_open.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o skeleton_open $(INC_LIB) \
	skeleton_open.o SoKnot.o $(LIBS)

skeleton: skeleton.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o skeleton $(INC_LIB) skeleton.o SoKnot.o $(LIBS)

multiview: multiview.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o multiview $(INC_LIB) multiview.o SoKnot.o $(LIBS)

merge_all: merge_all.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o merge_all $(INC_LIB) merge_all.o SoKnot.o $(LIBS)

t: t.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o t $(INC_LIB) t.o SoKnot.o $(LIBS)

kv: knotview.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o kv $(INC_LIB) knotview.o SoKnot.o $(LIBS)

#kv4: knot4view.o SoKnot.o
#	@echo Link ${<:.o=}
#	@$(LINKER) $(FLAGS) -o kv4 $(INC_LIB) knot4view.o SoKnot.o $(LIBS)

xyzview: xyzview.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o xyzview $(INC_LIB) xyzview.o SoKnot.o $(LIBS)

curveview: curveview.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o curveview $(INC_LIB) curveview.o SoKnot.o $(LIBS)

main: SoKnot.o main.o Widgets.o moc_Widgets.o moc_main.o
	@echo Link $@
	$(LINKER) $(FLAGS) -o main $(INC_LIB) SoKnot.o main.o \
	Widgets.o moc_Widgets.o moc_main.o $(LIBS)

plotzoom: plotzoom.o moc_plotzoom.o
	@echo Link $@
	@$(LINKER) $(FLAGS) -o plotzoom $(INC_LIB) moc_plotzoom.o plotzoom.o $(LIBS)

moc_main.cpp: main.h
	moc -o $@ $<
moc_Widgets.cpp: Widgets.h
	moc -o $@ $<
moc_trefoil_builder.cpp: trefoil_builder.h
	moc -o $@ $<
moc_plotzoom.cpp: plotzoom.h
	moc -o $@ $<

trefoil_builder: SoKnot.o trefoil_builder.o moc_Widgets.o Widgets.o moc_trefoil_builder.o moc_main.o
	@echo Link $@
	@$(LINKER) $(FLAGS) -o trefoil_builder $(INC_LIB) SoKnot.o \
	Widgets.o moc_Widgets.o moc_trefoil_builder.o \
	trefoil_builder.o moc_main.o $(LIBS)

###--------------###
###    Clean     ###
###--------------###

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f $(OBJ) $(COMMON_OBJ) moc_*.o plotzoom.o Widget.o
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f moc_* moc_*.o
	@-rm -f $(OBJ) $(COMMON_OBJ) $(PROGS)
	@-rm -f *~ *% .*~ .*% core
	@-rm -rf ii_files
