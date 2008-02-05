include config

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

test_double_critical: test_double_critical.cpp
	g++ -I${LIBBIARC}/include -I./include -o $@ $< \
	-L${LIBBIARC}/lib -lbiarc

realclean:
	-rm -f thick compute_thickness test_circle test_double_critical
