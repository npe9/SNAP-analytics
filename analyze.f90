PROGRAM analyze

  USE iso_c_binding

  IMPLICIT NONE

  INCLUDE 'mpif.h'

  INTEGER, PARAMETER :: i_knd = SELECTED_INT_KIND( 8 )

  INTEGER, PARAMETER :: r_knd = SELECTED_REAL_KIND( 13 )

  REAL(r_knd), PARAMETER :: zero = 0.0_r_knd

  REAL(r_knd), ALLOCATABLE,   DIMENSION(:,:,:,:) :: tmp

  REAL(r_knd), ALLOCATABLE, DIMENSION(:) :: pop, res

  REAL(r_knd), POINTER, DIMENSION(:,:,:,:) :: flux

  REAL(r_knd), POINTER, DIMENSION(:) :: v

  INTEGER(i_knd) :: g, ierr, ng, nx, ny, nz, cy, otno, root, nproc, iproc, fin

  REAL(r_knd) :: dx, dy, dz  

  CHARACTER(len=64) :: segment

  type(c_ptr) :: cptr1, cptr2
  ! do we need mpi here?
  !CALL MPI_INIT(ierr)

  root=0;
  nproc=0;
  WRITE(*,*) 'starting analysis'
  IF(IARGC() /= 1) THEN
    WRITE(*,*) 'usage: analyze'
    CALL EXIT(-1)
  END IF
  CALL getarg(1, segment)
  !read(arg,*) nproc

  !CALL MPI_COMM_SIZE ( MPI_COMM_WORLD, nproc, ierr )
  !CALL MPI_COMM_RANK ( MPI_COMM_WORLD, iproc, ierr )

  WRITE(*,*) 'setting up'
  CALL setup(nx, ny, nz, ng, dz, dy, dz, iproc, segment, LEN(segment))

  WRITE(*,*) 'allocating basics'
  ALLOCATE( tmp(nx,ny,nz,ng), STAT=ierr)
  ALLOCATE( pop(ng), STAT=ierr)
  ALLOCATE( res(ng), STAT=ierr)

  WRITE(*,*) 'doing C allocation'
  CALL allocate(cptr1, cptr2, cy, fin)

  WRITE(*,*) 'doing loop'
  loop: DO WHILE ( fin == 0)
     CALL C_F_POINTER(cptr1, flux, [nx,ny,nz,ng])
     CALL C_F_POINTER(cptr2, v, [ng])

  WRITE(*,*) 'iteration'
     pop=zero
     WRITE(*, *) 'popped'
     !tmp = flux*dx*dy*dz
    WRITE(*, *) 'tmp'
     DO g=1, ng
        WRITE(*, *) 'popping'
        pop(g) = SUM(tmp(:,:,:,g))
     END DO

     !CALL MPI_REDUCE(pop, res, ng, MPI_REAL8, MPI_SUM, root, MPI_COMM_WORLD, ierr)

!     pop = pop/v

     IF(iproc == root) THEN
        WRITE ( *,* ) cy,":"
        DO g=1, ng
           WRITE ( *,* ) pop(g)
        END DO
    END IF
    
  WRITE(*,*) 'deallocate'
     CALL deallocate
     
  WRITE(*,*) 'new allocation'
     CALL allocate(cptr1, cptr2, cy, fin)
  END DO loop
  
  WRITE(*,*) 'deallocating'
  CALL deallocate

  WRITE(*,*) 'closing'
  CALL close
  !CALL MPI_FINALIZE ( ierr )
END PROGRAM analyze
