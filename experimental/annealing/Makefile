all: fit
	g++ -DMAIN my_anneal.cpp -o my_anneal

fit: fit.cpp my_anneal.cpp
	g++ -L. -I. -o fit fit.cpp

trefoil_torus: trefoil_torus.cpp
	g++ -L${LIBBIARC}/lib -I${LIBBIARC}/include -lbiarc -lrt -o $@ $<

torus_trefoil_test: torus_trefoil_test.cpp
	g++ -L${LIBBIARC}/lib -I${LIBBIARC}/include -lbiarc -lrt -o $@ $<

link_anneal: link_anneal.cpp
	g++ -L${LIBBIARC}/lib -I${LIBBIARC}/include -lbiarc -lrt -o $@ $<
