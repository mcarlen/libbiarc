DIRS=lib tools objects inventor # temp test

all:
	@for dir in $(DIRS); do \
		cd $$dir && { make all; cd ..; } \
	done

test:
	cd tests; $(MAKE)

clean:
	@for dir in $(DIRS); do \
		cd $$dir && { make clean; cd ..; } \
	done

library:
	@echo Build biarc library; \
	cd lib/ && { make; cd ..; } ; \

rendermanlib:
	@echo Build biarc lib with renderman support; \
	cd lib/ && { make renderman; cd ..; } ;

realclean:
	@for dir in $(DIRS) benchmark annealing; do \
		cd $$dir && { make realclean; cd ..; } \
	done
	@echo Remove doc
	@rm -rf ./doc
	@echo Clean include
	@cd include/ && { rm -f *~ *% .*~ .*% core ; \
        rm -rf ii_file; cd ..; } ;

meshonly:
	@echo Build biarc library; \
	cd lib/ && { make; cd ..; } ; \
	echo making mesh4stokes in directory tools; \
	cd tools/ && { make mesh4stokes perturb map resample; cd ..; }

stokes: meshonly
	@echo
	@echo In the directory lib/ you will now find
	@echo either the shared library libbiarc.so
	@echo or the static lib libbiarc.a
	@echo
	@echo Add the lib/ directory to your
	@echo LD_LIBRARY_PATH environment variable
	@echo if you use the shared lib
	@echo
	@echo The program that generates the mesh
	@echo points for the stokes program is
	@echo
	@echo tools/mesh4stokes
	@echo

doc:
	@echo Generate documentation
	@doxygen doxygen.conf # >/dev/null 2>&1; \

lcvmdoc: doc
	@echo Copy documentation to ~/public_html/libbiarc
	@rm -r ~/public_html/libbiarc && { cp -r ./doc/html ~/public_html/libbiarc; }
