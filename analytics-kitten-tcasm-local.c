#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

struct usda {
	int outer;
	int time_loop;
	int finalized;
	unsigned long long size;
	size_t offset;
	int n[4];
	double d[3];
};

sem_t *lock;
char lock_name[10] = "", filename[20] = "";
struct usda *cte;
void *data;
int fd;

int setup_(int *nx, int *ny, int *nz, int *ng, double *dx, double *dy, double *dz, int *iproc) {
  // need to open and attach to the kitten tcasm.
  // so how do you do that?
  // I need to know the kitten values.
  // this is interesting.
  // so I forget, how do I give access?
  
  // do I need to model the concurrency?
  //
  // what I map in is the struct. If I am using my tcasm or xpmem I do that.
  // so when we get the struct
	cte = (struct usda *) mmap(NULL, sizeof(struct usda), PROT_READ, MAP_PRIVATE, fd, 0);	
	while(cte->time_loop == 0) {
		sleep(1);
    // so we "get" the value each time.
    // and remap based on what we had before.
		munmap(cte,  sizeof(struct usda));
		cte = (struct usda *) mmap(NULL, sizeof(struct usda), PROT_READ, MAP_PRIVATE, fd, 0);
	}

	*nx = cte->n[0];
	*ny = cte->n[1];
	*nz = cte->n[2];
	*ng = cte->n[3];

	*dx = cte->d[0];
	*dy = cte->d[1];
	*dz = cte->d[2];
	
	return 1;
}

int allocate_(void **flux, void **v, int *cy, int *fin) {
	size_t size1 = 8, size2 = 8*cte->n[3];
	int i;

  // so this is for making the variables.
  // they aren't sharing these are they?
	cte = (struct usda *) mmap(NULL, sizeof(struct usda), PROT_READ, MAP_PRIVATE, fd, 0);

	for(i=0; i < 4; i++)
		size1 *= cte->n[i];
 	
	*flux = mmap(NULL, size1, PROT_READ, MAP_PRIVATE, fd, cte->offset);
	*v = mmap(NULL, size2, PROT_READ, MAP_PRIVATE, fd, cte->offset + size1);

	*cy = cte->time_loop;
	*fin = cte->finalized;

	return 1;
}

int unlink_() {
  // unlink all resources
	return 1;
}

int close_(){
  // close all resources
	return 1;
}
