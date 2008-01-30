include config

thick: thick.cpp
	g++ -I${LIBBIARC}/include -I${KNOTLIB}/include \
	-I./include -o $@ thick.cpp \
	${LIBBIARC}/lib/libbiarc.a ${KNOTLIB}/lib/libknot.a -lrt

clean:
	-rm -f thick
