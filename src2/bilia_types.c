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

#include "bilia_types.h"

BILIA_VECTOR_DEFINE (bilia_uid_array, uid_t);

BILIA_VECTOR_DEFINE (bilia_uint_array, uint_t);

BILIA_VECTOR_DEFINE (bilia_type_array, bilia_type_t *);


/* ====================================================================== */
/* Globals */
/* ====================================================================== */

bilia_type_t *
bilia_mk_type_bool ()
{
  bilia_type_t *ret = (bilia_type_t *) malloc (sizeof (struct bilia_type_t));
  ret->kind = BILIA_TYP_BOOL;
  ret->args = bilia_uid_array_new ();
  return ret;
}

bilia_type_t *
bilia_mk_type_int ()
{
  bilia_type_t *ret = (bilia_type_t *) malloc (sizeof (struct bilia_type_t));
  ret->kind = BILIA_TYP_INT;
  ret->args = bilia_uid_array_new ();
  return ret;
}

bilia_type_t *
bilia_mk_type_w ()
{
  bilia_type_t *ret = (bilia_type_t *) malloc (sizeof (struct bilia_type_t));
  ret->kind = BILIA_TYP_W;
  ret->args = bilia_uid_array_new ();
  return ret;
}

bilia_type_t *
bilia_mk_type_bagint ()
{
  bilia_type_t *ret = (bilia_type_t *) malloc (sizeof (struct bilia_type_t));
  ret->kind = BILIA_TYP_BAGINT;
  ret->args = bilia_uid_array_new ();
  return ret;
}

bilia_type_t *
bilia_type_clone (bilia_type_t * a)
{
  if (!a)
    return a;
  bilia_type_t *ret = (bilia_type_t *) malloc (sizeof (struct bilia_type_t));
  ret->kind = a->kind;
  ret->args = bilia_uid_array_new ();
  bilia_uid_array_copy (ret->args, a->args);
  return ret;
}

void
bilia_type_free (bilia_type_t * a)
{
  if (!a)
    return;
  if (a->args)
    bilia_uid_array_delete (a->args);
  free (a);
}

void
bilia_type_fprint (FILE * f, bilia_type_t * a)
{
  if (a == NULL)
    fprintf (f, "(null)");
  switch (a->kind)
    {
    case BILIA_TYP_BOOL:
      fprintf (f, "Bool");
      break;
    case BILIA_TYP_INT:
      fprintf (f, "() Int");
      break;
    case BILIA_TYP_W:
      fprintf (f, "() Int");
      break;
    case BILIA_TYP_BAGINT:
      fprintf (f, "(Int) Int");
      break;
    default:
      fprintf (f, "(unknown)");
      break;
    }
}

/* ====================================================================== */
/* Other methods */

/* ====================================================================== */

bool
bilia_type_is_vartype (bilia_type_t * t)
{
  assert (NULL != t);
  /// depends on logic  // TODO NEW
  if ((t->kind >= BILIA_TYP_INT))
    return true;
  return false;
}
