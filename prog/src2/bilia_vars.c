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

#include "bilia_vars.h"
void breakpointnil(){}

BILIA_VECTOR_DEFINE (bilia_var_array, bilia_var_t *);

/* ====================================================================== */
/* Constructors/destructors */

/* ====================================================================== */

bilia_var_t *
bilia_var_new (const char *name, bilia_type_t * ty, bilia_scope_e s)
{
  bilia_var_t *v = (bilia_var_t *) malloc (sizeof (bilia_var_t));
  v->vname = strdup (name);
  v->vty = ty;
  v->scope = s;
  v->used = false;
  v->element = false;
  v->extremum = false;
  return v;
}

void
bilia_var_free (bilia_var_t * a)
{
  if (!a)
    return;
  if (a->vname)
    free (a->vname);
  /*if (a->vty)
    bilia_type_free (a->vty);*/
  free (a);
}

bilia_var_t *
bilia_var_copy (bilia_var_t * a)
{
  assert (a != NULL);
  bilia_var_t *r = (bilia_var_t *) malloc (sizeof (bilia_var_t));
  r->vid = a->vid;
  r->vname = strdup (a->vname);
  r->vty = bilia_type_clone (a->vty);
  r->scope = a->scope;
  return r;
}

bilia_var_array *
bilia_var_array_make (uid_t sz)
{
  bilia_var_array *a = bilia_var_array_new ();
  if (sz > 0)
    bilia_var_array_reserve (a, sz);
  return a;
}

void
bilia_var_register (bilia_var_array * a, const char *name, bilia_type_t * ty,
                   bilia_scope_e scope,bool element)
{
  assert (NULL != ty);
  assert (bilia_type_is_vartype (ty) == true);

  bilia_var_t *v = bilia_var_new (name, ty, scope);
  v->scope = scope;
  bilia_var_array_push (a, v);
  v->vid = bilia_vector_size (a) - 1;
  v->element = element;
  return;
}

uid_t
bilia_var_array_find_local (bilia_var_array * a, const char *name)
{
  if (a && (bilia_vector_size (a) > 0))
    {
      uid_t sz = bilia_vector_size (a);
      for (uid_t i = 0; i < sz; i++)
        if (bilia_vector_at (a, i) && !strcmp (name,
                                              bilia_vector_at (a, i)->vname))
          return i;
    }
  breakpointnil();
  return UNDEFINED_ID;
}

/** Used to obtain the name of a variable from an identifier.
 * @param a   local environment, if NULL search in global environment
 * @param vid variable identifier
 */
char *
bilia_var_name (bilia_var_array * a, uid_t vid, bilia_typ_t ty)
{
  if (&ty != &ty)
    {
      assert (0);               // just to avoid an "unused parameter warning"
    }

  if (a == NULL)
    return "unknown";
  if (vid == VNIL_ID)
    {
      breakpointnil();
      return "nil";
    }
  if (vid >= bilia_vector_size (a))
    {
      printf
        ("bilia_var_name: called with identifier %d not in the local environment.\n",
         vid);
      return "unknown";
    }
  return (bilia_vector_at (a, vid))->vname;
}

void
bilia_var_array_fprint (FILE * f, bilia_var_array * a, const char *msg,bool b)
{
  fprintf (f, "\n");
  fflush (f);
  if (!a)
    {
      /* fprintf (f, "null\n"); */
      return;
    }
  /* fprintf (f, "["); */
  uid_t length_a = bilia_vector_size (a);
  for (uid_t i = 0; i < length_a; i++)
    {
      bilia_var_t *vi = bilia_vector_at (a, i);
      assert (vi != NULL);
      bilia_type_t *ti = vi->vty;
      if(!vi->used && vi->vty->kind != BILIA_TYP_W) continue;
      if(ti->kind == BILIA_TYP_BAGINT && !b)continue;
      if (vi->scope == BILIA_SCOPE_LOCAL)
        fprintf (f, "?");
      else
        fprintf (f, "(declare-fun ");
      fprintf (f, "%s ", vi->vname);
      bilia_type_fprint (f, ti);
      fprintf (f, ")\n");
    }
  /* fprintf (f, " - ]"); */
  fflush (f);
}
