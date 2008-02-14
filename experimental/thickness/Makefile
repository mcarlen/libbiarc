include config

LIBBIARC=../../

thick: thick.cpp
	g++ -Wall -I${LIBBIARC}/include -I${KNOTLIB}/include \
	-o $@ thick.cpp \
	${LIBBIARC}/lib/libbiarc.a ${KNOTLIB}/lib/libknot.a -lrt

compute_thickness: compute_thickness.cpp
	g++ -Wall -I${LIBBIARC}/include -o $@ $< \
	-L${LIBBIARC}/lib -lbiarc

test_circle: test_circle.cpp
	g++ -I${LIBBIARC}/include -o $@ $< \
	-L${LIBBIARC}/lib -lbiarc

double_critical: double_critical.cpp
	g++ -Wall -I${KNOTLIB}/include -I../../include -o $@ $< \
	-L../../lib -lbiarc ${KNOTLIB}/lib/libknot.a

realclean:
	-rm -f thick compute_thickness test_circle double_critical
