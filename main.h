#include <stdlib.h>

/* MPI-related */
#define ROOT 0

/* Data structures constants */
#define INIT_SIZE 10
#define GROWTH_FACTOR 1.5


typedef struct
{
  size_t size;
  void *data;
} Variable;


typedef struct
{
  int id;
  int nb_id;
  Variable *data;
  size_t size;
} Process;
