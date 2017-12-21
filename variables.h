#ifndef VARIABLES_H
#  define VARIABLES_H

#  include "super_malloc.h"

// Extract the process id, and the variable id in the process data table
unsigned long pid_part(unsigned long var_id);
unsigned long vid_part(unsigned long var_id);

// Allocate a variable in a process and returns its id.
unsigned long var_alloc(Process *me);
// Read the value af a variable previously allocated
int var_get(Process *me, unsigned long var_id);
// Write value to variable
int var_set(Process *me, unsigned long var_id, int new_value);
// Free variable
void var_free(Process *me, unsigned long var_id);
// Pretty print var_id
void print_var_id(unsigned long var_id);

#endif // VARIABLES_H
