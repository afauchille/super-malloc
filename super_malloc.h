#ifndef SUPER_MALLOC_H
#  define SUPER_MALLOC_H

#  include <stdlib.h>
#  include <stdio.h>
#  include <mpi.h>

/* MPI-related */
#  define ROOT 0

/* Data structures constants */
#  define INIT_SIZE 10
#  define GROWTH_FACTOR 1.5


typedef struct
{
  size_t size;
  int *data;
} Variable;


typedef struct
{
  int id;
  int nb_id;
  Variable *data;
  size_t size;
  size_t capacity;
} Process;

/*************
 * Variables *
 *************/


#endif // SUPER_MALLOC_H
