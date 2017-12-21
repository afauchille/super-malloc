#ifndef VARIABLES_H
#  define VARIABLES_H

#  include "super_malloc.h"

unsigned long pid_part(unsigned long var_id);
unsigned long vid_part(unsigned long var_id);

unsigned long var_alloc(Process *me);
int var_get(Process *me, unsigned long var_id);
int var_set(Process *me, unsigned long var_id, int new_value);
void var_free(Process *me, unsigned long var_id);
void print_var_id(unsigned long var_id);

#endif // VARIABLES_H
