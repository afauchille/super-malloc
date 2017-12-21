#include "super_malloc.h"
#include "variables.h"

/*******************
 * Process helpers *
 *******************/

/* Initialize MPI and return process structure containg relevant data */
Process p_init(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  Process me;
  MPI_Comm_rank(MPI_COMM_WORLD, &me.id); // using rank as an id
  MPI_Comm_size(MPI_COMM_WORLD, &me.nb_id); // world size is thereby the number of ids

  me.data = (Variable*)malloc(INIT_SIZE * sizeof(Variable));
  me.size = 0;
  me.capacity = INIT_SIZE;
  return me;
}

void p_finalize(Process *me)
{
  for (size_t i = 0; i < me->size; ++i)
    if (me->data[i].data)
      free(me->data[i].data);
  free(me->data);
}

/* Simple wrapper for sending one int */
void simple_send(int *data, int id_dest)
{
  //printf("Sending from %d to %d\n", *data, id_dest); // debug
  MPI_Send(data, 1, MPI_INT, id_dest, 0, MPI_COMM_WORLD);
}

/* Simple wrapper for receiving one int */
void simple_recv(int *data, int id_from, int id) // last param only used in print
{
  //printf("Receiving from %d to %d\n", id_from, id); // debug
  MPI_Recv(data, 1, MPI_INT, id_from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}



/****************************************/

void check_tree(Process *me)
{
  int data = -1;
  for (int i = 1; i < me->nb_id; i *= 2)
  {
    if (me->id < i / 2)
      ; // already done, test is for clarity purpose
    else if (me->id < i) {
      for (int id_dest = 2 * me->id + 1; id_dest < 2 * me->id + 3; ++id_dest)
	if (id_dest < me->nb_id)
	  simple_send(&me->id, id_dest);
    }
    else if (me->id < 2 * i)
      simple_recv(&data, (me->id - 1) / 2, me->id);
  }

  MPI_Barrier(MPI_COMM_WORLD); // sync all processes
  printf("[%d]: %d is my dad.\n", me->id, data);
}

int main(int argc, char **argv)
{
  Process process = p_init(argc, argv);
  Process *me = &process;

  check_tree(me);

  if (me->id == 2)
  {
    unsigned long var_id = var_alloc(me);
    print_var_id(var_id);
    var_set(me, var_id, 42);
    printf("Value: %d\n", var_get(me, var_id));
    var_set(me, var_id, 70);
    printf("Value: %d\n", var_get(me, var_id));
    var_free(me, var_id);
  }

  p_finalize(me);

  MPI_Finalize ();

  return 0;
}
