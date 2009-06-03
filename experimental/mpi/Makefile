LIBBIARC=../../

len: len.cpp
	mpicxx -DTIMING -I../../include -L../../lib -o len len.cpp -lbiarc -lrt
	echo "run with : mpirun -machinefile machines -np 4 ./len"

len2: len2.cpp
	g++ -I../../include -L../../lib $< -o $@ -lbiarc -lrt

a: a.cpp
	g++ -I../../include -L../../lib $< -o $@ -lbiarc -lrt


run: len
	mpirun -np 4 ./len

test:
	mpicc -o test test.cpp
	@echo "run with : mpirun -machinefile machines -np 2 ./test"


realclean:
	-rm -f test
