#include "super_malloc.h"
#include "variables.h"


/***************
 * MPI helpers *
 ***************/

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
  me.s1min_size = 0;
  me.s2min_size = 0;
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

void simple_send_UL(unsigned long *data, int id_dest, int id)
{
  //  printf("Sending (%lu, %lu) from %d to %d\n", pid_part(*data), vid_part(*data), id, id_dest);
  MPI_Send(data, 1, MPI_UNSIGNED_LONG, id_dest, 0, MPI_COMM_WORLD);
}

void send_sons(int *data1, int *data2, Process *me)
{
  int son1 = 2 * me->id + 1;
  int son2 = son1 + 1;
  if (son1 < me->nb_id)
    simple_send(data1, son1);
  if (son2 < me->nb_id)
    simple_send(data2, son2);
}

void send_sons_same_value(int *data, Process *me)
{
  send_sons(data, data, me);
}


/* Simple wrapper for receiving one int */
void simple_recv(int *data, int id_from, int id) // last param only used in print
{
  //printf("Receiving from %d to %d\n", id_from, id); // debug
  MPI_Recv(data, 1, MPI_INT, id_from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void simple_recv_UL(unsigned long *data, int id_from, int id) // last param only used in print
{
  // printf("Receiving from %d to %d\n", id_from, id); // debug
  MPI_Recv(data, 1, MPI_UNSIGNED_LONG, id_from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //  printf("Receiving from %d to %d Done.\n", id_from, id); // debug
}

void recv_sons_UL(unsigned long *data1, unsigned long *data2, Process *me)
{
  int son1 = 2 * me->id + 1;
  int son2 = son1 + 1;
  if (son1 < me->nb_id)
    simple_recv_UL(data1, son1, me->id);
  if (son2 < me->nb_id)
    simple_recv_UL(data2, son2, me->id);
}

int up_pow2(int n)
{
  //  n -= 1;
  int pow = 0;
  while (n >>= 1)
    pow++;
  return 1 << (pow + 1);
}



/****************************************/

void check_tree(Process *me)
{
  int data = -1;
  for (int i = 1; i < me->nb_id; i *= 2)
  {
    if (me->id < i / 2)
      ; // already done, test is for clarity purpose
    else if (me->id < i)
      send_sons_same_value(&me->id, me);
    else if (me->id < 2 * i)
      simple_recv(&data, (me->id - 1) / 2, me->id);
  }

  MPI_Barrier(MPI_COMM_WORLD); // sync all processes
  printf("[%d]: %d is my dad.\n", me->id, data);
}

unsigned long alloc(Process *me)
{
  unsigned long var_id = -1;
  int status = 0;
  if (me->id == ROOT)
    status = 1; // kind of a token, passing from root to target
  for (int i = 1; i < me->nb_id * 2; i *= 2) // * 2 in condition is here to make sure all processes go in the first else if (and therefore that leaves can take the alloc)
  {
    if (me->id < i / 2)
      ; // already done, test is for clarity purpose
    else if (me->id < i)
    {
      int data1 = 0;
      int data2 = 0;
      if (status == 1) // choose if making the alloc or passing the order to sons
      {
	status = 0;
	if (me->size <= min(me->s1min_size, me->s2min_size)) // take the alloc
	  status = 2;
	else if (me->s1min_size <= me->s2min_size) // forward to son1
	  data1 = 1;
	else                                       // forward to son2
	  data2 = 1;
      }
      send_sons(&data1, &data2, me);
    }
    else if (me->id < 2 * i)
      simple_recv(&status, (me->id - 1) / 2, me->id);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (status == 2) {
    //    printf("[%d] I take the alloc\n", me->id);
    var_id = var_alloc(me);
  }
  else if (status == 1)
    printf("[%d] Something went wrong\n", me->id);

  for (int i = up_pow2(me->nb_id); i != 2; i /= 2)
  {
    int ri = i - 2; // correction to get correct max index: 0, 2, 6, 14, ...
    if (me->id > i - 2)
      ; // do nothing
    else if (me->id >= (i - 2) / 2)
      simple_send_UL(&var_id, (me->id - 1) / 2, me->id);
    else if (me->id > i / 4 - 2)
    {
      unsigned long data1 = -1;
      unsigned long data2 = -1;
      recv_sons_UL(&data1, &data2, me);
      if (data1 != (unsigned long)-1)
	{
	  //printf("[%d] Raising s1min_size\n", me->id);
	  me->s1min_size += 1;
	}
      else if (data2 != (unsigned long)-1)
	{
	  //printf("[%d] Raising s2min_size\n", me->id);
	  me->s2min_size += 1;
	}
      var_id = min(var_id, min(data1, data2));
    }
  }

  if (var_id != (unsigned long)-1 && me->id == ROOT)
    printf("[%d] Var id:(%lu,%lu)\n", me->id, pid_part(var_id), vid_part(var_id));

  return var_id;
}

int main(int argc, char **argv)
{
  Process process = p_init(argc, argv);
  Process *me = &process;

  //check_tree(me);

  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  alloc(me);
  
  /*for (int i = up_pow2(me->nb_id); i != 2; i /= 2)
    {
      int ri = i - 2;
      if (me->id >= ri)
	; // do nothing
      else if (me->id >= ri / 2)
	printf("%d -> %d\n", me->id, (me->id - 1) / 2);
      else if (me->id >= ri / 4)
	printf("%d <- %d\n%d <- %d\n", me->id, me->id * 2 + 1, me->id, me->id * 2 + 2);
	}*/

  p_finalize(me);

  MPI_Finalize ();

  return 0;
}
