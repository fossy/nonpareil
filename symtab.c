/*
 * symtab.h
 *
 * CASM is an assembler for the processor used in the HP "Classic" series
 * of calculators, which includes the HP-35, HP-45, HP-55, HP-65, HP-70,
 * and HP-80.
 *
 * Copyright 1995 Eric Smith
 */

#include "symtab.h"

void init_symbol_table (void)
{
}

/* returns 1 for success, 0 if duplicate name */
int create_symbol (char *name, int value)
{
  return 1;
}

/* returns 1 for success, 0 if not found */
int lookup_symbol (char *name, int *value)
{
  *value = 0;
  return 0;
}

