/**************************************************************************
 *
 *  BaTS decision procedure
 *
 *  you can redistribute it and/or modify it under the terms of the GNU
 *  Lesser General Public License as published by the Free Software
 *  Foundation, version 3.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  See the GNU Lesser General Public License version 3.
 *  for more details (enclosed in the file LICENSE).
 *
 **************************************************************************/

/**
 * Variables for BILIA.
 */

#ifndef _BILIA_VARS_H_
#define _BILIA_VARS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <limits.h>

#include "bilia_types.h"

/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

#define VNIL_ID UNDEFINED_ID

/**
 * Visibility flag values
 */
  typedef enum
  {
    BILIA_SCOPE_LOCAL = 0, BILIA_SCOPE_GLOBAL, BILIA_SCOPE_OTHER
  } bilia_scope_e;

/** Variable information for both locations and set of locations variables:
 * - the name of the variable in the program
 * - the type(s) of the variable
 * - flag for local or global
 */
  typedef struct bilia_var_t
  {
    char *vname;                // declaration name
    uid_t vid;                  // variable identifier
    bilia_type_t *vty;           // type
    bilia_scope_e scope;         // visibility
    bool used ;
    bool element;
  } bilia_var_t;

/** Type of the global array of variables. */
    BILIA_VECTOR_DECLARE (bilia_var_array, bilia_var_t *);

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

/*
 * Global variables are stored within the formulae,
 * in fields lvars and svars of bilia_form_t.
 * They have the flag scope set to BILIA_SCOPE_GLOBAL.
 */

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */
  bilia_var_t *bilia_var_new (const char *name, bilia_type_t * vty,
                            bilia_scope_e s);
/* Build a variable record. */
  void bilia_var_free (bilia_var_t * a);
/* Free a variable record. */
  bilia_var_t *bilia_var_copy (bilia_var_t * a);
/* Makes a copy of the variable. */

  bilia_var_array *bilia_var_array_make (uint_t sz);
/* Allocate an array of size variables.
 The variables are initialized with NULL pointers. */

/* ====================================================================== */
/* Other methods */
/* ====================================================================== */

  void bilia_var_register (bilia_var_array * a, const char *name,
                          bilia_type_t * ty, bilia_scope_e s,bool element);
/* Add a variable declaration to the array a. */

  uint_t bilia_var_array_find_local (bilia_var_array * a, const char *name);
/* Search the position of the variable name in the local array a. */

  char *bilia_var_name (bilia_var_array * a, uid_t vid, bilia_typ_t ty);
/* ====================================================================== */
/* Printing */
/* ====================================================================== */

  void bilia_var_array_fprint (FILE * f, bilia_var_array * a, const char *msg, bool b);
/* Print the array a. */

#ifdef __cplusplus
}
#endif

#endif                          /* _BILIA_VARS_H_ */
