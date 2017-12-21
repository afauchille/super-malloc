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

/* OP macro funcion */
#  define min(X, Y) ((X) < (Y) ? (X) : (Y))

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
  size_t s1min_size;
  size_t s2min_size;
} Process;

/*************
 * Variables *
 *************/


#endif // SUPER_MALLOC_H
