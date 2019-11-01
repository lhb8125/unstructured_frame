CXX=mpicxx.mpich
#CXX=/home/export/online1/systest/swrh/lhb/software/OpenMPI/bin/mpicxx
#CXX=/home/export/online1/systest/swrh/lhb/software/MPICH/bin/mpicxx

LINKFLAG=-fPIC

DEPFLAG=-MMD -MP

CXXFLAG=-O2

LIB=-lyaml-cpp -lcgns -lhdf5 -lparmetis -lmetis -lm -lz -ldl

INCLUDE=-I/home/liuhb/Downloads/yaml-cpp-release-0.3.0/build/include \
	-I/home/liuhb/software/CGNS-3.4.0/src/build-hdf5/include \
	-I./ \
        -I./loadBalancer/ \
	-I./mesh/ \
	-I./region/ \
	-I./topology  

LIBDIR=-L/home/liuhb/Downloads/yaml-cpp-release-0.3.0/build/lib \
       -L/home/liuhb/software/CMake-hdf5-1.10.5/build/lib \
       -L/home/liuhb/software/CGNS-3.4.0/src/build-hdf5/lib

CXXOBJS=loadBalancer/loadBalancer.o test/test.o mesh/section.o \
		topology/topology.o mesh/mesh.o mesh/nodes.o

DEPS:=$(CXXOBJS:.o=.d)

EXE=test.out

${EXE}: ${CXXOBJS}
	${CXX} ${LINKFLAG} -o ${EXE} $^ ${INCLUDE} ${LIBDIR} ${LIB}

${CXXOBJS}:%.o:%.cpp
	${CXX} ${CXXFLAG} ${DEPFLAG} -c $< -o $@ ${INCLUDE}

clean:
	rm -f ${EXE} ${CXXOBJS}

-include $(DEPS)

#g++ -c loadBalancer/loadBalancer.cpp ${INCLUDE} -o loadBalancer/loadBalancer.o
#g++ -c test/test.cpp -I./ -I./loadBalancer/ ${INCLUDE} -o test.o
#g++ -o test.out test.o loadBalancer/loadBalancer.o ${INCLUDE} ${LIB} -lyaml-cpp
#./test.out
