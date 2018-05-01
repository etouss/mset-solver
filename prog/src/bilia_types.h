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
 * Type system for BILIA.
 */

#ifndef _BILIA_TYPES_H_
#define _BILIA_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bilia_vector.h"

/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

/** Identifiers type */
  typedef uid_t uid_t;

/** Constant used to signal an undefined identifier */
#define UNDEFINED_ID UINT32_MAX

/** Vector of identifiers */
    BILIA_VECTOR_DECLARE (bilia_uid_array, uid_t);

/** Vector of integers */
    BILIA_VECTOR_DECLARE (bilia_uint_array, uint_t);

/** Basic types used in BILIA */
  typedef enum
  {
    BILIA_TYP_BOOL = 0,
    BILIA_TYP_INT,
    BILIA_TYP_BAGINT,
    BILIA_TYP_W,
    BILIA_TYP_OTHER
  } bilia_typ_t;

/** Type expression.
 */
  typedef struct bilia_type_t
  {
    bilia_typ_t kind;
    bilia_uid_array *args;       // type arguments, including the record index
  } bilia_type_t;

  bilia_type_t * tybag;
  bilia_type_t * tybool;
  bilia_type_t * tyint;

/** Type of the global array of records. */
    BILIA_VECTOR_DECLARE (bilia_type_array, bilia_type_t *);

#define BILIA_TYP_VOID 0

  bilia_type_t *bilia_mk_type_bool (void);
  bilia_type_t *bilia_mk_type_int (void);
  bilia_type_t *bilia_mk_type_w (void);
  bilia_type_t *bilia_mk_type_bagint (void);
/* Constructors for the predefined types. */
  bilia_type_t *bilia_type_clone (bilia_type_t * a);
  void bilia_type_free (bilia_type_t * a);
  void bilia_type_fprint (FILE * f, bilia_type_t * a);

/* ====================================================================== */
/* Other methods */
/* ====================================================================== */

  bool bilia_type_is_vartype (bilia_type_t * t);
/* True if type may be used for a variable */

#ifdef __cplusplus
}
#endif

#endif                          /* _NOL_TYPES_H_ */
