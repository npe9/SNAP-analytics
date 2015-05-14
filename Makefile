all: analytics.c analyze.f90
	mpicc -c analytics.c &&\
	mpif90 -fopenmp -std=gnu -c analyze.f90
	mpif90 -fopenmp -std=gnu -o msync_anaylze  analytics.o analyze.o -lpthread -lrt
clean: 
	rm -f msync_anaylze *.o
