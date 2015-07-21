#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <xemem.h>

struct usda {
	int outer;
	int time_loop;
	int finalized;
	unsigned long long size;
	size_t offset;
	int n[4];
	double d[3];
	void *flux;
	void *v;
	int niter;
	int mine;
};

sem_t *lock;
char lock_name[10] = "", filename[20] = "";
struct usda *cte;
void *data;
int myiter;

int setup_(int *nx, int *ny, int *nz, int *ng, double *dx, double *dy, 
		double *dz, int *iproc, char *fseg, int len) {
	xemem_segid_t segid;
	xemem_apid_t apid;
	struct xemem_addr addr;
	void *v;
	int i;
	char *segment;

	printf("fixing fseg %s with len %d\n",fseg, len);
	fseg[23]=0;
	segment = fseg;
	printf("looking up segid %s len %d\n", segment, strlen(segment));
	// add some sort of synchronization here? There's a race between creation and use.
		
	segid = xemem_lookup_segid(segment);
	i = 1000;
	while(segid < 0 && i--){
		sleep(1);
		segid = xemem_lookup_segid(segment);
	}

	if(segid < 0) {
		printf("failed to lookup segment\n");
		exit(-1);
	}

	printf("xemem getting\n");
	apid = xemem_get(segid, XEMEM_RDWR);
	if(apid < 0) {
		printf("failed to lookup apid for segment %ld\n", segid);
		exit(-1);
	}

	addr.apid = apid;
	addr.offset = 0;
	// round up?
	printf("xemem attaching\n");
	cte = (struct usda *)xemem_attach(addr, sizeof(struct usda), NULL);
	if(cte == NULL) {
		printf("could not attach to addr apid %lx offset %ld\n", 
				addr.apid, addr.offset);
		exit(-1);
	}
// I want to check on how signalling versus polling works, we're going to poll
// now.	
	//while(cte->time_loop == 0) {
		// how do you find this?
		// what I'm doing is actually a signal wait.
		// we're reopening the shared memory here.
		// that is not necessary.
	//}

	*nx = cte->n[0];
	*ny = cte->n[1];
	*nz = cte->n[2];
	*ng = cte->n[3];

	*dx = cte->d[0];
	*dy = cte->d[1];
	*dz = cte->d[2];

	myiter = cte->niter;
	cte->mine = 1;
	printf("done setting up\n");
	// need to set up the fluxes
	return 1;
}

int allocate_(void **flux, void **v, int *cy, int *fin) {
	size_t size1 = 8, size2 = 8*cte->n[3];
	int i;

	printf("allocating\n");
	// poll for a new update
	while(myiter == cte->niter && !cte->finalized){
		printf("waiting for iter myiter %d cte->niter %d cte->mine %d \n", myiter, cte->niter, cte->mine);
	}
		printf("got myiter %d cte->niter %d cte->mine %d cte->finalized %d\n", myiter, cte->niter, cte->mine, cte->finalized);
	// XXX: this is a botch fix it later
	if(cte->finalized)
		exit(1);
	printf("it's mine again\n");
	cte->mine = 1;
	printf("got an update\n");
	myiter = cte->niter;
	for(i=0; i < 4; i++)
		size1 *= cte->n[i];
	// so you are allocating two things here.
	// but what you're really doing is selecting shared memory.
	*flux = cte->flux;
	*v = cte->v;

	//*flux = mmap(NULL, size1, PROT_READ, MAP_PRIVATE, fd, cte->offset);
	//*v = mmap(NULL, size2, PROT_READ, MAP_PRIVATE, fd, cte->offset + size1);

	*cy = cte->time_loop;
	*fin = cte->finalized;

	printf("finished allocating\n");
	return 1;
}

int deallocate_() {
	printf("unlinking from shared region\n");
	return 1;
}

int close_(){
	printf("closing\n");
	for(;;);
	return 1;
}
