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
	sprintf(filename, "/%di.shm", *iproc);
	sprintf(lock_name, "/sem_lock%d", *iproc);

	lock = sem_open(lock_name, 0);
	while (lock == SEM_FAILED) {
		sleep(1);
		lock = sem_open(lock_name, 0);
	}
	sem_wait(lock);

	fd = shm_open(filename, O_RDONLY, 0666);
	while (fd < 0){
		printf("failed to open file\n");
		sleep(1);
		fd = shm_open(filename, O_RDONLY, 0666);
	}

	cte = (struct usda *) mmap(NULL, sizeof(struct usda), PROT_READ, MAP_PRIVATE, fd, 0);
	while(cte->time_loop == 0) {
		sleep(1);
		fd = shm_open(filename, O_RDONLY, 0666);
		while (fd < 0){
			printf("failed to open file\n");
			sleep(1);
			fd = shm_open(filename, O_RDONLY, 0666);
		}
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

int shm_allocate_(void **flux, void **v, int *cy, int *fin) {
	size_t size1 = 8, size2 = 8*cte->n[3];
	int i;

	sem_wait(lock);

	cte = (struct usda *) mmap(NULL, sizeof(struct usda), PROT_READ, MAP_PRIVATE, fd, 0);

	for(i=0; i < 4; i++)
		size1 *= cte->n[i];

	*flux = mmap(NULL, size1, PROT_READ, MAP_PRIVATE, fd, cte->offset);
	*v = mmap(NULL, size2, PROT_READ, MAP_PRIVATE, fd, cte->offset + size1);

	*cy = cte->time_loop;
	*fin = cte->finalized;

	return 1;
}

int unlink_shm_() {
	munmap(data, cte->size);
	munmap(cte,  sizeof(struct usda));
	return 1;
}

int shm_close_(){
	shm_unlink(filename);
	sem_close(lock);
	sem_unlink(lock_name);
	return 1;
}
