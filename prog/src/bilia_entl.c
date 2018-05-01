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
 * Defines the problem for the decision procedure.
 */

#include <sys/time.h>
#include <stdio.h>

#include "bilia_entl.h"

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

bilia_entl_t *bilia_prob;         // problem of entailment in bilia

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */

/* Initialization/Deallocation of problem */
void
bilia_entl_init (void)
{
  bilia_prob = (bilia_entl_t *) malloc (sizeof (bilia_entl_t));
  // init file name
  bilia_prob->smt_fname = NULL;
  bilia_prob->output_fname = NULL;

  bilia_prob->lvars = bilia_var_array_new ();
  bilia_prob->wvars = bilia_var_array_new ();
  // init positive formula
  //bilia_prob->pform = bilia_form_new ();

  // init negative formulae array
  bilia_prob->nform = bilia_dform_array_new ();
  bilia_prob->wform = bilia_dform_array_new ();
  bilia_prob->rform = bilia_dform_array_new ();
  //bilia_form_array_push(bilia_prob->nform,bilia_form_new());
  //bilia_prob->rform = bilia_form_new ();
  //bilia_prob->wform = bilia_form_new ();

  // if empty aray, then SAT problem, else ENTAILMENT problem

  // init command
  bilia_prob->cmd = BILIA_FORM_SAT;       // by default value
  }

/**
 * Free memory allocated for entailment checking
 */

void
bilia_entl_free (void)
{
  assert (bilia_prob != NULL);
  // this procedure shall be called only once
  if (bilia_prob->smt_fname != NULL)
    {
      free (bilia_prob->smt_fname);
      bilia_prob->smt_fname = NULL;
    }
 if (bilia_prob->nform != NULL)
    {
      for (size_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->nform); i++)
        bilia_dform_free(bilia_vector_at(bilia_prob->nform,i));
      bilia_dform_array_delete (bilia_prob->nform);
      bilia_prob->nform = NULL;
    }
if (bilia_prob->wform != NULL)
   {
     for (size_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->wform); i++)
       bilia_dform_free(bilia_vector_at(bilia_prob->wform,i));
     bilia_dform_array_delete (bilia_prob->wform);
     bilia_prob->wform = NULL;
   }
if (bilia_prob->rform != NULL)
  {
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->rform); i++)
      bilia_dform_free(bilia_vector_at(bilia_prob->rform,i));
    bilia_dform_array_delete (bilia_prob->rform);
    bilia_prob->rform = NULL;
  }
  if (bilia_prob->lvars != NULL)
    {
      for (size_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->lvars); i++)
        bilia_var_free(bilia_vector_at(bilia_prob->lvars,i));
      bilia_var_array_delete (bilia_prob->lvars);
      bilia_prob->lvars = NULL;
    }
  if (bilia_prob->wvars != NULL)
    {
      for (size_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->wvars); i++)
        bilia_var_free(bilia_vector_at(bilia_prob->wvars,i));
      bilia_var_array_delete (bilia_prob->wvars);
      bilia_prob->wvars = NULL;
    }
  free (bilia_prob);
}

/* ====================================================================== */
/* Getters */
/* ====================================================================== */


bilia_var_array *
bilia_entl_get_var_array ()
{
  assert (bilia_prob != NULL);
  return bilia_prob->lvars;
}

bilia_var_array *
bilia_entl_get_wvar_array ()
{
  assert (bilia_prob != NULL);
  return bilia_prob->wvars;
}


bilia_entl_t *
bilia_entl_get ()
{
  return bilia_prob;
}

bilia_dform_t *
bilia_entl_get_nform_last ()
{
  assert (bilia_prob != NULL);
  assert (bilia_prob->nform != NULL);
  if (bilia_vector_empty (bilia_prob->nform))
    bilia_dform_array_push (bilia_prob->nform, bilia_dform_new ());

  return bilia_vector_last (bilia_prob->nform);
}

bilia_dform_array *
bilia_entl_get_nform ()
{
  assert (bilia_prob != NULL);
  return bilia_prob->nform;
}

bilia_dform_array *
bilia_entl_get_wform ()
{
  assert (bilia_prob != NULL);
  return bilia_prob->wform;
}

bilia_dform_array *
bilia_entl_get_rform ()
{
  assert (bilia_prob != NULL);
  return bilia_prob->rform;
}



/* ====================================================================== */
/* Setters */
/* ====================================================================== */

void
bilia_entl_set_fname (char *fname)
{
  assert (bilia_prob->smt_fname == NULL);
  bilia_prob->smt_fname = strdup (fname);
}

void
bilia_entl_set_foutput (char *fname)
{
  assert (bilia_prob->output_fname == NULL);
  bilia_prob->output_fname = strdup (fname);
}

void
bilia_entl_set_cmd (bilia_form_kind_t pb)
{
  bilia_prob->cmd = pb;
}


/* ====================================================================== */
/* Predicates */
/* ====================================================================== */
int
bilia_entl_is_sat (void)
{
  /* assert (bilia_prob != NULL); */

  /* if (bilia_prob->nform == NULL || bilia_vector_empty (bilia_prob->nform)) */
    return 1;
  /* return 0; */
}


/* ====================================================================== */
/* Printers */
/* ====================================================================== */

void
bilia_entl_fprint (FILE * f)
{
  assert (f != NULL);

  if (bilia_prob == NULL)
    {
      fprintf (f, "*** (entailment) null\n");
      return;
    }
  /* bilia_fields_array_fprint (f, "fields:"); */
  bilia_var_array_fprint (f, bilia_prob->lvars, "lvars",true);
  bilia_var_array_fprint (f, bilia_prob->wvars, "wvars",true);
  bilia_dform_array_fprint (f, bilia_prob->lvars,bilia_prob->nform,0);
  bilia_dform_array_fprint (f, bilia_prob->lvars,bilia_prob->rform,0);
  //bilia_dform_array_fprint (f, bilia_prob->lvars,bilia_prob->wform,0);
  fflush (f);

}

/* ====================================================================== */
/* Solver */
/* ====================================================================== */

int
bilia_entl_solve (void)
{
  //int res = 0;
   /*
   * Special case of sat solving, when no negative formula
   */
  /* if (bilia_entl_is_sat ()) */
    return bilia_sat_solve ();

}
