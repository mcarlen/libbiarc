include config

LIBBIARC=../../

thick: thick.cpp
	g++ -I${LIBBIARC}/include -I${KNOTLIB}/include \
	-I./include -o $@ thick.cpp \
	${LIBBIARC}/lib/libbiarc.a ${KNOTLIB}/lib/libknot.a -lrt

compute_thickness: compute_thickness.cpp
	g++ -I${LIBBIARC}/include -I./include -o $@ $< \
	-L${LIBBIARC}/lib -lbiarc

test_circle: test_circle.cpp
	g++ -I${LIBBIARC}/include -I./include -o $@ $< \
	-L${LIBBIARC}/lib -lbiarc

double_critical: double_critical.cpp
	g++ -I${KNOTLIB}/include -I../../include -I./include -o $@ $< \
	-L../../lib -lbiarc ${KNOTLIB}/lib/libknot.a

realclean:
	-rm -f thick compute_thickness test_circle test_double_critical
