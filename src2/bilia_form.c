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
 * Abstract Syntax Tree of BILIA formulas.
 */

#include <sys/time.h>

#include "bilia_form.h"
#include "bilia_entl.h"
/* #include "bilia2sat.h" */

typedef unsigned int uint;

BILIA_VECTOR_DEFINE (bilia_dterm_array, bilia_dterm_t *);

BILIA_VECTOR_DEFINE (bilia_dform_array, bilia_dform_t *);

//BILIA_VECTOR_DEFINE (bilia_form_array, bilia_form_t *);

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

bilia_logic_t bilia_form_logic;

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */


bilia_dterm_t *
bilia_dterm_new (void)
{
  bilia_dterm_t *ret = (bilia_dterm_t *) malloc (sizeof (struct bilia_dterm_s));
  ret->kind = BILIA_DATA_INT;
  ret->typ = BILIA_TYP_INT;
  ret->p.value = 0;
  ret->args = NULL;
  return ret;
}
bilia_dterm_t * bilia_dterm_copy (bilia_dterm_t * term){
  bilia_dterm_t *ret = (bilia_dterm_t *) malloc (sizeof (struct bilia_dterm_s));
  ret->kind = term->kind;
  ret->typ = term->typ;
  switch(ret->kind){
    case BILIA_DATA_ITE:
      {
        ret->p.cond = bilia_dform_copy(term->p.cond);
        ret->args = bilia_dterm_array_new();
        for ( uint_t i = 0; i < BILIA_VECTOR_SIZE(term->args); i++)
          bilia_dterm_array_push(ret->args,bilia_dterm_copy(bilia_vector_at(term->args,i)));
      }
      break;
    case BILIA_DATA_VAR:
      {
        ret->p.sid = term->p.sid;
        ret->args = NULL;
      }
      break;
    case BILIA_DATA_INT:
      {
        ret->p.value = term->p.value;
        ret->args = NULL;
      }
      break;
    case BILIA_DATA_EMPTYBAG:
        {
          ret->p.value = 0;
          ret->args = NULL;
        }
        break;
    default:
      {
        ret->p.value = term->p.value;
        ret->args = bilia_dterm_array_new();
        for ( uint_t i = 0; i < BILIA_VECTOR_SIZE(term->args); i++)
          bilia_dterm_array_push(ret->args,bilia_dterm_copy(bilia_vector_at(term->args,i)));
      }
    }
    return ret;
}

bilia_dterm_t *
bilia_dterm_new_var (uint_t vid, bilia_typ_t ty)
{
  bilia_dterm_t *ret = (bilia_dterm_t *) malloc (sizeof (struct bilia_dterm_s));
  ret->kind = BILIA_DATA_VAR;
  ret->typ = ty;
  ret->p.sid = vid;
  ret->args = NULL;
  return ret;
}

void
bilia_dterm_free (bilia_dterm_t * d)
{
  if (d == NULL)
    return;
  else if (d->kind != BILIA_DATA_VAR && d->kind != BILIA_DATA_INT && d->kind != BILIA_DATA_EMPTYBAG)
  {
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(d->args); i++)
      bilia_dterm_free(bilia_vector_at(d->args,i));
    bilia_dterm_array_delete (d->args);
    d->args = NULL;
  }
  // printf("%d\n",d->kind);
  // fflush(stdout);
  free (d);
}

bilia_dform_t *
bilia_dform_new (void)
{
  bilia_dform_t *ret = (bilia_dform_t *) malloc (sizeof (struct bilia_dform_s));
  ret->kind = BILIA_DATA_EMPTYBAG;
  ret->typ = BILIA_TYP_BAGINT;
  ret->p.targs = NULL;
  return ret;
}

bilia_dform_t *bilia_dform_copy (bilia_dform_t *form){
  bilia_dform_t *ret = (bilia_dform_t *) malloc (sizeof (struct bilia_dform_s));
  ret->kind = form->kind;
  ret->typ = form->typ;
  switch(ret->kind)
  {
    case BILIA_DATA_AND:
    case BILIA_DATA_OR:
    case BILIA_DATA_NOT:
    case BILIA_DATA_IMPLIES:
    {
      ret->p.bargs = bilia_dform_array_new();
      for ( uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.bargs); i++)
        bilia_dform_array_push(ret->p.bargs,bilia_dform_copy(bilia_vector_at(form->p.bargs,i)));
    }
    break;
    default:
    {
      ret->p.targs = bilia_dterm_array_new();
      for ( uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.targs); i++)
      bilia_dterm_array_push(ret->p.targs,bilia_dterm_copy(bilia_vector_at(form->p.targs,i)));
    }
  }
  return ret;
}

bilia_dform_t *
bilia_dform_new_eq (bilia_dterm_t * t1, bilia_dterm_t * t2)
{
  assert (t1 != NULL);
  assert (t2 != NULL);
  assert (t1->typ == t2->typ);
  bilia_dform_t *ret = (bilia_dform_t *) malloc (sizeof (struct bilia_dform_s));
  ret->kind = BILIA_DATA_EQ;
  ret->typ = t1->typ;
  ret->p.targs = bilia_dterm_array_new ();
  bilia_dterm_array_push (ret->p.targs, t1);
  bilia_dterm_array_push (ret->p.targs, t2);
  return ret;
}

void
bilia_dform_free (bilia_dform_t * d)
{
  if (d == NULL)
    return;
  if (d->kind != BILIA_DATA_IMPLIES && d->kind != BILIA_DATA_OR && d->kind != BILIA_DATA_AND && d->kind != BILIA_DATA_NOT)
    {
      if (d->p.targs != NULL){
        for (size_t i = 0; i < BILIA_VECTOR_SIZE(d->p.targs); i++){
          // bilia_dterm_fprint(stdout,bilia_entl_get_var_array(),bilia_vector_at(d->p.targs,i),0);
          // printf("\n");
          // fflush(stdout);
          bilia_dterm_free(bilia_vector_at(d->p.targs,i));
          bilia_vector_at(d->p.targs,i) = NULL;
        }
        bilia_dterm_array_delete (d->p.targs);
        d->p.targs = NULL;
      }
    }
  else
    {
      if (d->p.bargs != NULL){
        for (size_t i = 0; i < BILIA_VECTOR_SIZE(d->p.bargs); i++){
          bilia_dform_free(bilia_vector_at(d->p.bargs,i));
          bilia_vector_at(d->p.bargs,i) = NULL;
        }
        bilia_dform_array_delete (d->p.bargs);
        d->p.bargs = NULL;
      }
    }
  free (d);
}


/* ====================================================================== */
/* Printing */

/* ====================================================================== */

void
bilia_dterm_fprint (FILE * f, bilia_var_array * lvars, bilia_dterm_t * dt,int level)
{

  if (dt == NULL)
    {
      fprintf (f, "null");
      return;
    }

  switch (dt->kind)
    {
    case BILIA_DATA_INT:
      fprintf (f, "%ld", dt->p.value);
      break;
    case BILIA_DATA_VAR:
      if(dt->typ == BILIA_TYP_W)  fprintf (f, "%s", bilia_var_name (bilia_entl_get_wvar_array(),dt->p.sid, dt->typ));
      else fprintf (f, "%s", bilia_var_name (lvars, dt->p.sid, dt->typ));
      break;
    case BILIA_DATA_EMPTYBAG:
      fprintf (f, "emptybag");
      break;
    case BILIA_DATA_PLUS:
      fprintf (f, "(+ ");
      break;
    case BILIA_DATA_MINUS:
      fprintf (f, "(- ");
      break;
    case BILIA_DATA_BAG:
      fprintf (f, "(bag ");
      break;
    case BILIA_DATA_BAGN:
      fprintf (f, "(bagn ");
      break;
    case BILIA_DATA_BAGUNION:
      fprintf (f, "(bagunion ");
      break;
    case BILIA_DATA_BAGINTER:
      fprintf (f, "(baginter ");
      break;
    case BILIA_DATA_BAGSUM:
      fprintf (f, "(bagsum ");
      break;
    case BILIA_DATA_BAGMINUS:
      fprintf (f, "(bagminus ");
      break;
    case BILIA_DATA_BAGMAX:
      fprintf (f, "(bagmax ");
      break;
    case BILIA_DATA_BAGMIN:
      fprintf (f, "(bagmin ");
      break;
    case BILIA_DATA_MIN:
      fprintf (f, "(min ");
      break;
    case BILIA_DATA_MAX:
      fprintf (f, "(max ");
      break;
    case BILIA_DATA_ITE:
       fprintf (f, "(ite\n");
       bilia_dform_fprint (f, lvars, dt->p.cond,level+1);
       fprintf (f, " ");
       break;
    case BILIA_DATA_APP:
      fprintf (f, "(");
      break;
    default:
      fprintf (f, "(error)");
      break;
    }

  if (dt->args != NULL)
    {
      for (uint_t i = 0; i < bilia_vector_size (dt->args); i++)
        {
          bilia_dterm_fprint (f, lvars, bilia_vector_at (dt->args, i),level);
          fprintf (f, " ");
        }
      fprintf (f, ")");
    }
}

void
bilia_dform_fprint (FILE * f, bilia_var_array * lvars, bilia_dform_t * df,int level)
{
  if (df == NULL)
    {
      fprintf (f, "null\n");
      return;
    }
    for (int i = 0; i < level; i++)
      fprintf (f, "\t");
  if (df->kind == BILIA_DATA_AND)
    {
      fprintf (f, "(and\n");
      for(uint_t i=0;i<BILIA_VECTOR_SIZE(df->p.bargs);i++){
        bilia_dform_fprint (f, lvars, bilia_vector_at (df->p.bargs, i),level+1);
        fprintf (f, "\n");
      }
      for (int i = 0; i < level; i++)
        fprintf (f, "\t");
      fprintf (f, ")");
      return;
    }
  else if (df->kind == BILIA_DATA_OR)
    {
      fprintf (f, "(or\n ");
      for(uint_t i=0;i<BILIA_VECTOR_SIZE(df->p.bargs);i++){
        bilia_dform_fprint (f, lvars, bilia_vector_at (df->p.bargs, i),level+1);
        fprintf (f, "\n");
      }
      for (int i = 0; i < level; i++)
        fprintf (f, "\t");
      fprintf (f, ")");
      return;
    }
  else if (df->kind == BILIA_DATA_IMPLIES)
    {
      fprintf (f, "(=>\n");
      bilia_dform_fprint (f, lvars, bilia_vector_at (df->p.bargs, 0),level+1);
      fprintf(f,"\n");
      bilia_dform_fprint (f, lvars, bilia_vector_at (df->p.bargs, 1),level+1);
      fprintf (f, ")");
      return;
    }
  else if (df->kind == BILIA_DATA_NOT)
    {
      fprintf (f, "(not ");
      bilia_dform_fprint (f, lvars, bilia_vector_at (df->p.bargs, 0),level);
      fprintf (f, ")");
      return;
    }
  switch (df->kind)
    {
    case BILIA_DATA_EQ:
      fprintf (f, "(= ");
      break;
    case BILIA_DATA_NEQ:
      fprintf (f, "(distinct ");
      break;
    case BILIA_DATA_LT:
      fprintf (f, "(< ");
      break;
    case BILIA_DATA_GT:
      fprintf (f, "(> ");
      break;
    case BILIA_DATA_LE:
      fprintf (f, "(<= ");
      break;
    case BILIA_DATA_GE:
      fprintf (f, "(>= ");
      break;
    case BILIA_DATA_IN:
      fprintf (f, "(in ");
      break;
    case BILIA_DATA_INN:
      fprintf (f, "(inn ");
      break;
    case BILIA_DATA_NIN:
      fprintf (f, "(nin ");
      break;
    case BILIA_DATA_NI:
      fprintf (f, "(ni ");
      break;
     case BILIA_DATA_NSUBSETEQ:
      fprintf (f, "(nsubseteq ");
      break;
    case BILIA_DATA_SUBSETEQ:
      fprintf (f, "(subseteq ");
      break;
    default:
      break;                    /// print only the term
    }
  if (df->p.targs != NULL)
    for (uint_t i = 0; i < bilia_vector_size (df->p.targs); i++)
      {
        bilia_dterm_fprint (f, lvars, bilia_vector_at (df->p.targs, i),level);
        fprintf (f, " ");
      }
  fprintf (f, ")");
}

void
bilia_dform_array_fprint (FILE * f, bilia_var_array * lvars, bilia_dform_array * df,int level)
{
  if (df == NULL)
    {
      fprintf (f, "null\n");
      return;
    }
  for (uint_t i = 0; i < bilia_vector_size (df); i++)
    {
      fprintf (f, "\n\n");
      bilia_dform_fprint (f, lvars, bilia_vector_at (df, i),level);
    }
  fprintf (f, "\n\n");
}
