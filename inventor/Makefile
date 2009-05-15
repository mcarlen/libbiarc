PROGS = xyzview main animate plotzoom # kv4

include ../Makefile.config

######### INCLUDES ###########################

COIN=`coin-config --prefix`
SOQT=`soqt-config --prefix`
INCLUDE  = -I../include -I$(COIN)/include -I$(SOQT)/include -I/usr/X11R6/include
INC_LIB = `soqt-config --ldflags` -L../lib -L$(COIN)/lib -L$(SOQT)/lib -L/usr/lib -L/usr/local/lib -L/usr/X11R6/lib
######### LIBRARIES  #########################

LIBS = `soqt-config --libs` -lbiarc_float

######### FLAGS      #########################

FLAGS = $(INCLUDE) `soqt-config --cflags` `pkg-config --cflags QtCore QtGui` `soqt-config --cxxflags` $(CXXFLAGS) -DDEBUG -DPRECISION_HACK -Wall # -D_USE_QT_  -D__debug__

ifeq ($(RENDERMAN),YES)
INCLUDE+= -I${PIXIEHOME}/include
INC_LIB+= -L${PIXIEHOME}/lib
LIBS+= -lri
endif



######### SOURCE FILES #######################

SRC = SoKnot.cpp xyzview.cpp Widgets.cpp \
	main.cpp trefoil_builder.cpp animate.cpp \
        plotzoom.cpp
OBJ = $(SRC:.cpp=.o)

#####  RULES  ####
.SUFFIXES: .cpp 

.cpp.o:
	@echo Compile $<
	$(CXX) ${FLAGS} -c $<

################ COMPILATION #####################

all: $(PROGS)

t: t.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o t $(INC_LIB) $? $(LIBS)

#kv4: knot4view.o SoKnot.o
#	@echo Link ${<:.o=}
#	@$(LINKER) $(FLAGS) -o kv4 $(INC_LIB) knot4view.o SoKnot.o $(LIBS)

xyzview: xyzview.o SoKnot.o
	@echo Link ${<:.o=}
	@$(LINKER) $(FLAGS) -o xyzview $(INC_LIB) $? $(LIBS)

fourier_syn.o: ../fourierknots/fourier_syn.cpp
	@echo Compile $<
	$(CXX) ${FLAGS} -o $@ -c $<

main: SoKnot.o main.o Widgets.o moc_Widgets.o qrc_application.o utils.o mainwindow.o moc_mainwindow.o gui.o fourier_syn.o
	@echo Link $@
	$(LINKER) $(FLAGS) -o main $(INC_LIB) SoKnot.o main.o Widgets.o moc_Widgets.o qrc_application.o utils.o mainwindow.o moc_mainwindow.o gui.o fourier_syn.o $(LIBS)

plotzoom: plotzoom.o moc_plotzoom.o
	@echo Link $@
	@echo $(LINKER) $(FLAGS) -o plotzoom $(INC_LIB) $? $(LIBS)
	@$(LINKER) $(FLAGS) -o plotzoom $(INC_LIB) $? $(LIBS)

animate: SoKnot.o animate.o 
	@echo Link $@
	$(LINKER) $(FLAGS) -o $@ $(INC_LIB) $^ $(LIBS)

qrc_application.cpp: application.qrc
	rcc -o $@ $<
moc_%.cpp: %.h
	moc -o $@ $<

trefoil_builder: SoKnot.o trefoil_builder.o moc_Widgets.o Widgets.o moc_trefoil_builder.o moc_main.o
	@echo Link $@
	@$(LINKER) $(FLAGS) -o trefoil_builder $(INC_LIB) $< $(LIBS)

###--------------###
###    Clean     ###
###--------------###

clean :
	@echo Cleaning `basename ${PWD}`
	@-rm -f $(OBJ) $(COMMON_OBJ) moc_*.o plotzoom.o animate.o *.o
	@-rm -rf ii_files

realclean :
	@echo Purge `basename ${PWD}`
	@-rm -f moc_* moc_*.o
	@-rm -f $(OBJ) $(COMMON_OBJ) $(PROGS)
	@-rm -f *~ *% .*~ .*% core
	@-rm -rf ii_files
