#include <stdio.h>
#include <mpi.h>

#define ROOT size - 1

int main(int argc, char **argv)
{
  int rank, size, len;
  char version [MPI_MAX_LIBRARY_VERSION_STRING];
  MPI_Init(& argc , &argv ) ;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data = -1;

  if (rank == ROOT)
  {
    data = rank;
  }

  MPI_Bcast(&data, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  printf("[%d]: %d is my master.\n", rank, data);

  MPI_Finalize ();

  return 0;
}
