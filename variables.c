#include "variables.h"

inline
unsigned long pid_part(unsigned long var_id) { return var_id >> 24; }
inline
unsigned long vid_part(unsigned long var_id) { return var_id & 0xFF; }

unsigned long var_alloc(Process *me)
{
  if (me->size == me->capacity - 1)
  {
    me->capacity *= GROWTH_FACTOR;
    me->data = (Variable*)realloc(me->data, me->capacity * sizeof(Variable));
  }
  unsigned long var_id = (me->id << 24) | me->size;
  Variable *var = me->data + me->size++;
  var->size = 1;
  var->data = malloc(sizeof(int));
  return var_id;
}

int var_get(Process *me, unsigned long var_id)
{
  unsigned long i = vid_part(var_id);
  if (i > me->size) {
    fprintf(stderr, "Variable requested with wrong var_id: process %lu, %luth variable\n", pid_part(var_id), i);
    return -1;
  }
  Variable var = me->data[i];
  if (!var.data) {
    fprintf(stderr, "Variable requested for already freed vaiable: process %lu, %luth variable\n", pid_part(var_id), i);
    return -1;
  }
  if (var.size == 1)
    return *var.data;
  return -1; // size > 1 not implemented yet
}

// Warning: not thread-safe-or-whatever-it-is-called-in-the-mpi-world
int var_set(Process *me, unsigned long var_id, int new_value)
{
  unsigned long i = vid_part(var_id);
  if (i > me->size) {
    fprintf(stderr, "Variable requested with wrong var_id: process %lu, %luth variable\n", pid_part(var_id), i);
    return 0;
  }
  Variable var = me->data[i];
  if (!var.data) {
    fprintf(stderr, "Variable requested for already freed vaiable: process %lu, %luth variable\n", pid_part(var_id), i);
    return 0;
  }

  *var.data = new_value;
  return 1;
}

void var_free(Process *me, unsigned long var_id)
{
  unsigned long i = vid_part(var_id);
  if (i > me->size) {
    fprintf(stderr, "Variable free requested with wrong var_id: process %lu, %luth variable\n", pid_part(var_id), i);
    return;
  }
  if (!me->data[i].data) {
    fprintf(stderr, "Variable free requested for already freed vaiable: process %lu, %luth variable\n", pid_part(var_id), i);
    return;
  }
  Variable *var = me->data + i;
  var->data = NULL;
  free(var->data);
}

void print_var_id(unsigned long var_id)
{
  printf("Process %lu, %luth variable\n", pid_part(var_id), vid_part(var_id));
}
