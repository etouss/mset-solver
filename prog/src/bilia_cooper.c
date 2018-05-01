
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
 * Rewritting formula.
 */

#include "bilia_cooper.h"
extern bilia_type_t * tybool;
extern bilia_type_t * tybag;
extern bilia_type_t * tyint;

void breakpointC() {};

void neq_form(bilia_dform_t  *f){
  switch(f->kind){
    case BILIA_DATA_EQ:
      f->kind = BILIA_DATA_NEQ;
      break;
    case BILIA_DATA_GE:
      f->kind = BILIA_DATA_LT;
      break;
    case BILIA_DATA_GT:
      f->kind = BILIA_DATA_LE;
      break;
    case BILIA_DATA_LE:
      f->kind = BILIA_DATA_GT;
      break;
    case BILIA_DATA_LT:
      f->kind = BILIA_DATA_GE;
      break;
    case BILIA_DATA_NEQ:
      f->kind = BILIA_DATA_EQ;
      break;
  }
}

void bilia_cooper_emptybag(){
  bilia_var_array * lvars = bilia_entl_get_var_array();
  bilia_dterm_t * empty = bilia_dterm_new_var(bilia_var_array_find_local(lvars,"emptybag"),BILIA_TYP_BAGINT);
  for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
        {
          bilia_var_t * var = bilia_vector_at(lvars, j);
          if (var->vty->kind == BILIA_TYP_INT )
          { 
            bilia_dterm_t * left = bilia_dterm_new();
            left->kind = BILIA_DATA_APP;
            left->typ = BILIA_TYP_INT;
            left->args = bilia_dterm_array_new();
            bilia_dterm_array_push(left->args, empty);
            bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_t * right = bilia_dterm_new();
            right->kind = BILIA_DATA_INT;
            right->typ = BILIA_TYP_INT;
            right->p.value = 0;
            bilia_pure_add_dform(bilia_entl_get_rform()->pure, bilia_dform_new_eq(left,right));
          }
        }
}

void bilia_cooper_pn(bool pos)
{
  bilia_dform_array * df;
  if (pos) df = bilia_entl_get_pform()->pure->data;
  else df = bilia_entl_get_nform()->pure->data;

  bilia_form_t * rpform = bilia_form_new();
  bilia_var_array * lvars = bilia_entl_get_var_array();
  /* rcform->pure = bilia_pure_new(bilia_vector_size(bilia_entl_get_rform()->pure->data) * bilia_vector_size(lvars)); */
  rpform->pure = bilia_pure_new(100);
  for (uint_t i = 0; i < bilia_vector_size (df); i++)
  {
    bilia_dform_t * dform = bilia_vector_at(df, i);
    if (dform->kind == BILIA_DATA_SUBSET)
    {
      bilia_dterm_t * t0 = bilia_vector_at (dform->p.targs, 0);
      assert(t0->kind == BILIA_DATA_VAR);
      bilia_dterm_t * t1 = bilia_vector_at (dform->p.targs, 1);
      assert(t1->kind == BILIA_DATA_VAR);
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {
        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT )
        {

          bilia_dform_t *le = bilia_dform_new();
          le->kind = BILIA_DATA_LE;
          le->typ = BILIA_TYP_INT;
          le->p.targs = bilia_dterm_array_new ();

          bilia_dterm_t * left = bilia_dterm_new();
          left->kind = BILIA_DATA_APP;
          left->typ = BILIA_TYP_INT;
          left->args = bilia_dterm_array_new();
          bilia_dterm_array_push(left->args, t0);
          bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * right = bilia_dterm_new();
          right->kind = BILIA_DATA_APP;
          right->typ = BILIA_TYP_INT;
          right->args = bilia_dterm_array_new();
          bilia_dterm_array_push(right->args, t1);
          bilia_dterm_array_push(right->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_array_push(le->p.targs, left);
          bilia_dterm_array_push(le->p.targs, right);
          bilia_pure_add_dform(rpform->pure, le);
        }
      }
    }
    else if (dform->kind == BILIA_DATA_EQ)
    {
      bilia_dterm_t * t0 = bilia_vector_at (dform->p.targs, 0);
      assert(t0->kind == BILIA_DATA_VAR);
      bilia_dterm_t * t1 = bilia_vector_at (dform->p.targs, 1);
      assert(t1->kind == BILIA_DATA_VAR);
      if (t1->typ == BILIA_TYP_BAGINT)
      {
        for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
        {
          bilia_var_t * var = bilia_vector_at(lvars, j);
          if (var->vty->kind == BILIA_TYP_INT )
          { 
            bilia_dterm_t * left = bilia_dterm_new();
            left->kind = BILIA_DATA_APP;
            left->typ = BILIA_TYP_INT;
            left->args = bilia_dterm_array_new();
            bilia_dterm_array_push(left->args, t0);
            bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_t * right = bilia_dterm_new();
            right->kind = BILIA_DATA_APP;
            right->typ = BILIA_TYP_INT;
            right->args = bilia_dterm_array_new();
            bilia_dterm_array_push(right->args, t1);
            bilia_dterm_array_push(right->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));
            bilia_pure_add_dform(rpform->pure, bilia_dform_new_eq(left,right));
          }
        }
      }
      else  bilia_pure_add_dform(rpform->pure, dform);
    }
    else if (dform->kind == BILIA_DATA_IMPLIES)
    {
      bilia_dform_t *left_imp = bilia_vector_at(dform->p.bargs, 0);
      bilia_dform_t *right_imp = bilia_vector_at(dform->p.bargs, 1);

      bilia_dform_array * arrayR = bilia_dform_array_new();
      bilia_dform_array * arrayL = bilia_dform_array_new();

      if (left_imp->kind == BILIA_DATA_SUBSET)
      {
        bilia_dterm_t * t0 = bilia_vector_at (left_imp->p.targs, 0);
        assert(t0->kind == BILIA_DATA_VAR);
        bilia_dterm_t * t1 = bilia_vector_at (left_imp->p.targs, 1);
        assert(t1->kind == BILIA_DATA_VAR);
        for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
        {
          bilia_var_t * var = bilia_vector_at(lvars, j);
          if (var->vty->kind == BILIA_TYP_INT )
          {

            bilia_dform_t *le = bilia_dform_new();
            le->kind = BILIA_DATA_LE;
            le->typ = BILIA_TYP_INT;
            le->p.targs = bilia_dterm_array_new ();

            bilia_dterm_t * left = bilia_dterm_new();
            left->kind = BILIA_DATA_APP;
            left->typ = BILIA_TYP_INT;
            left->args = bilia_dterm_array_new();
            bilia_dterm_array_push(left->args, t0);
            bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_t * right = bilia_dterm_new();
            right->kind = BILIA_DATA_APP;
            right->typ = BILIA_TYP_INT;
            right->args = bilia_dterm_array_new();
            bilia_dterm_array_push(right->args, t1);
            bilia_dterm_array_push(right->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_array_push(le->p.targs, left);
            bilia_dterm_array_push(le->p.targs, right);

            bilia_dform_array_push(arrayL, le);
            /* bilia_pure_add_dform(rpform->pure, le); */
          }
        }

      }
      else
        bilia_dform_array_push(arrayL, left_imp);

      if (right_imp->kind == BILIA_DATA_SUBSET)
      {
        bilia_dterm_t * t0 = bilia_vector_at (right_imp->p.targs, 0);
        assert(t0->kind == BILIA_DATA_VAR);
        bilia_dterm_t * t1 = bilia_vector_at (right_imp->p.targs, 1);
        assert(t1->kind == BILIA_DATA_VAR);
        for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
        {
          bilia_var_t * var = bilia_vector_at(lvars, j);
          if (var->vty->kind == BILIA_TYP_INT )
          {

            bilia_dform_t *le = bilia_dform_new();
            le->kind = BILIA_DATA_GT;
            le->typ = BILIA_TYP_INT;
            le->p.targs = bilia_dterm_array_new ();

            bilia_dterm_t * left = bilia_dterm_new();
            left->kind = BILIA_DATA_APP;
            left->typ = BILIA_TYP_INT;
            left->args = bilia_dterm_array_new();
            bilia_dterm_array_push(left->args, t0);
            bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_t * right = bilia_dterm_new();
            right->kind = BILIA_DATA_APP;
            right->typ = BILIA_TYP_INT;
            right->args = bilia_dterm_array_new();
            bilia_dterm_array_push(right->args, t1);
            bilia_dterm_array_push(right->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

            bilia_dterm_array_push(le->p.targs, left);
            bilia_dterm_array_push(le->p.targs, right);

            bilia_dform_array_push(arrayR, le);
            /* bilia_pure_add_dform(rpform->pure, le); */
          }
        }

      }
      else
        /*NEGATION !!!!*/
        bilia_dform_array_push(arrayR, left_imp);

      breakpointC();

      for (uint_t r = 0; r < bilia_vector_size(arrayR); r++)
      {
        bilia_form_t * nnf = bilia_form_new();
        nnf->pure = bilia_pure_new(100);
        bilia_pure_add_dform(nnf->pure, bilia_vector_at(arrayR, r));
        bilia_vector_at(arrayR, r);
        for (uint_t l = 0; l < bilia_vector_size(arrayL); l++)
        {
          bilia_pure_add_dform(nnf->pure, bilia_vector_at(arrayL, l));
        }
        bilia_form_array_push(bilia_entl_get()->nform, nnf);
      }
    }
    else
    {
      bilia_pure_add_dform(rpform->pure, dform);
    }
  }


  if (pos)bilia_entl_get()->pform = rpform;
  else
  {
    bilia_entl_get()->nform = bilia_form_array_new();
    bilia_form_array_push(bilia_entl_get()->nform, rpform);
  }

}

void bilia_cooper_r()
{
  bilia_dform_array * df = bilia_entl_get_rform()->pure->data;
  bilia_var_array * lvars = bilia_entl_get_var_array();
  bilia_form_t * rcform = bilia_form_new();
  /* rcform->pure = bilia_pure_new(bilia_vector_size(bilia_entl_get_rform()->pure->data) * bilia_vector_size(lvars)); */
  rcform->pure = bilia_pure_new(100);
  if(df== NULL)return;
  for (uint_t i = 0; i < bilia_vector_size (df); i++)
  {
    bilia_dform_t * dform = bilia_vector_at(df, i);
    assert(dform->kind == BILIA_DATA_EQ);
    bilia_dterm_t * t0 = bilia_vector_at (dform->p.targs, 0);
    assert(t0->kind == BILIA_DATA_VAR);
    bilia_dterm_t * t1 = bilia_vector_at (dform->p.targs, 1);

    switch (t1->kind)
    {
    case BILIA_DATA_BAG:
    {
      /*t1 in t0*/
      bilia_dform_t *in = bilia_dform_new();
      in->kind = BILIA_DATA_EQ;
      in->typ = BILIA_TYP_INT;
      in->p.targs = bilia_dterm_array_new ();

      bilia_dterm_t * left = bilia_dterm_new();
      left->kind = BILIA_DATA_APP;
      left->typ = BILIA_TYP_INT;
      left->args = bilia_dterm_array_new ();
      bilia_dterm_array_push (left->args, t0);
      bilia_dterm_array_push (left->args, bilia_vector_at(t1->args, 0));

      bilia_dterm_t * right = bilia_dterm_new();
      right->kind = BILIA_DATA_INT;
      right->typ = BILIA_TYP_INT;
      right->p.value = 1;

      bilia_dterm_array_push(in->p.targs, left);
      bilia_dterm_array_push(in->p.targs, right);


      bilia_pure_add_dform(rcform->pure, in);
      /*diff imp ni*/
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {
        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT && var->vid != bilia_vector_at(t1->args, 0)->p.sid)
        {
          bilia_dform_t *imp = bilia_dform_new();
          imp->kind = BILIA_DATA_IMPLIES;
          imp->typ = BILIA_TYP_INT;
          imp->p.bargs = bilia_dform_array_new ();

          bilia_dform_t *impL = bilia_dform_new();
          impL->kind = BILIA_DATA_NEQ;
          impL->typ = BILIA_TYP_INT;
          impL->p.targs = bilia_dterm_array_new();
          bilia_dterm_array_push(impL->p.targs,bilia_vector_at(t1->args, 0));
          bilia_dterm_array_push(impL->p.targs, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));


          bilia_dterm_t * left = bilia_dterm_new();
          left->kind = BILIA_DATA_APP;
          left->typ = BILIA_TYP_INT;
          left->args = bilia_dterm_array_new ();
          bilia_dterm_array_push (left->args, t0);
          bilia_dterm_array_push (left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * right = bilia_dterm_new();
          right->kind = BILIA_DATA_INT;
          right->typ = BILIA_TYP_INT;
          right->p.value = 0;
          
          bilia_dform_array_push(imp->p.bargs,impL);
          bilia_dform_array_push(imp->p.bargs,bilia_dform_new_eq(left,right));
          bilia_pure_add_dform(rcform->pure, imp);
        }
      }
      break;
    }
    case BILIA_DATA_BAGMIN:
    {
      /*t0 in t1*/
      bilia_dform_t *in = bilia_dform_new();
      in->kind = BILIA_DATA_GT;
      in->typ = BILIA_TYP_INT;
      in->p.targs = bilia_dterm_array_new ();

      bilia_dterm_t * left = bilia_dterm_new();
      left->kind = BILIA_DATA_APP;
      left->typ = BILIA_TYP_INT;
      left->args = bilia_dterm_array_new ();
      bilia_dterm_array_push (left->args, bilia_vector_at(t1->args, 0));
      bilia_dterm_array_push (left->args, t0); 

      bilia_dterm_t * right = bilia_dterm_new();
      right->kind = BILIA_DATA_INT;
      right->typ = BILIA_TYP_INT;
      right->p.value = 0;

      bilia_dterm_array_push(in->p.targs, left);
      bilia_dterm_array_push(in->p.targs, right);


      bilia_pure_add_dform(rcform->pure, in);
      /*all ni si >=  donc =>*/
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {
        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT && var->vid != bilia_vector_at(t1->args, 0)->p.sid)
        {
          bilia_dform_t *imp = bilia_dform_new();
          imp->kind = BILIA_DATA_IMPLIES;
          imp->typ = BILIA_TYP_INT;
          imp->p.bargs = bilia_dform_array_new ();

          bilia_dform_t *left = bilia_dform_new();
          left->kind = BILIA_DATA_LT;
          left->typ = BILIA_TYP_INT;
          left->p.targs = bilia_dterm_array_new ();
          bilia_dterm_array_push (left->p.targs, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));
          bilia_dterm_array_push (left->p.targs, t0);
  
          bilia_dterm_t * lefteq = bilia_dterm_new();
          lefteq->kind = BILIA_DATA_APP;
          lefteq->typ = BILIA_TYP_INT;
          lefteq->args = bilia_dterm_array_new ();
          bilia_dterm_array_push (lefteq->args, bilia_vector_at(t1->args, 0));
          bilia_dterm_array_push (lefteq->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * righteq = bilia_dterm_new();
          righteq->kind = BILIA_DATA_INT;
          righteq->typ = BILIA_TYP_INT;
          righteq->p.value = 0;
          
          bilia_dform_array_push(imp->p.bargs, left);
          bilia_dform_array_push(imp->p.bargs, bilia_dform_new_eq(lefteq,righteq));
          bilia_pure_add_dform(rcform->pure, imp);
        }
      }
      break;
    }
    case BILIA_DATA_BAGMAX:
    {
      /*t0 in t1*/
      bilia_dform_t *in = bilia_dform_new();
      in->kind = BILIA_DATA_GT;
      in->typ = BILIA_TYP_INT;
      in->p.targs = bilia_dterm_array_new ();

      bilia_dterm_t * left = bilia_dterm_new();
      left->kind = BILIA_DATA_APP;
      left->typ = BILIA_TYP_INT;
      left->args = bilia_dterm_array_new ();
      bilia_dterm_array_push (left->args, bilia_vector_at(t1->args, 0));
      bilia_dterm_array_push (left->args, t0);
      

      bilia_dterm_t * right = bilia_dterm_new();
      right->kind = BILIA_DATA_INT;
      right->typ = BILIA_TYP_INT;
      right->p.value = 0;

      bilia_dterm_array_push(in->p.targs, left);
      bilia_dterm_array_push(in->p.targs, right);


      bilia_pure_add_dform(rcform->pure, in);

      /*all ni si >=  donc =>*/
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {
        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT && var->vid != bilia_vector_at(t1->args, 0)->p.sid)
        {
          bilia_dform_t *imp = bilia_dform_new();
          imp->kind = BILIA_DATA_IMPLIES;
          imp->typ = BILIA_TYP_INT;
          imp->p.bargs = bilia_dform_array_new ();

          bilia_dform_t *left = bilia_dform_new();
          left->kind = BILIA_DATA_GT;
          left->typ = BILIA_TYP_INT;
          left->p.targs = bilia_dterm_array_new ();
          bilia_dterm_array_push (left->p.targs, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));
          bilia_dterm_array_push (left->p.targs, t0);

          bilia_dterm_t * lefteq = bilia_dterm_new();
          lefteq->kind = BILIA_DATA_APP;
          lefteq->typ = BILIA_TYP_INT;
          lefteq->args = bilia_dterm_array_new ();
          bilia_dterm_array_push (lefteq->args, bilia_vector_at(t1->args, 0));
          bilia_dterm_array_push (lefteq->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * righteq = bilia_dterm_new();
          righteq->kind = BILIA_DATA_INT;
          righteq->typ = BILIA_TYP_INT;
          righteq->p.value = 0;
          
          bilia_dform_array_push(imp->p.bargs, left);
          bilia_dform_array_push(imp->p.bargs, bilia_dform_new_eq(lefteq,righteq));
          bilia_pure_add_dform(rcform->pure, imp);

        }
      }
      break;
    }
    case BILIA_DATA_BAGMINUS:
    {
      /*APP(t0 v) = max (APP(t1->0 v) APP(t1->1 v)) */
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {

        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT )
        {

          bilia_dterm_t * left = bilia_dterm_new();
          left->kind = BILIA_DATA_APP;
          left->typ = BILIA_TYP_INT;
          left->args = bilia_dterm_array_new();
          bilia_dterm_array_push(left->args, t0);
          bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * Rright = bilia_dterm_new();
          Rright->kind = BILIA_DATA_MAX;
          Rright->typ = BILIA_TYP_INT;
          Rright->args = bilia_dterm_array_new();
          bilia_dterm_t * righteq = bilia_dterm_new();
          righteq->kind = BILIA_DATA_INT;
          righteq->typ = BILIA_TYP_INT;
          righteq->p.value = 0;

          


          bilia_dterm_t * right = bilia_dterm_new();
          right->kind = BILIA_DATA_MINUS;
          right->typ = BILIA_TYP_INT;
          right->args = bilia_dterm_array_new();
          bilia_dterm_t * r0 = bilia_dterm_new();
          r0->kind = BILIA_DATA_APP;
          r0->typ = BILIA_TYP_INT;
          r0->args = bilia_dterm_array_new();
          bilia_dterm_array_push(r0->args, bilia_vector_at(t1->args, 0));
          bilia_dterm_array_push(r0->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));
          bilia_dterm_t * r1 = bilia_dterm_new();
          r1->kind = BILIA_DATA_APP;
          r1->typ = BILIA_TYP_INT;
          r1->args = bilia_dterm_array_new();
          bilia_dterm_array_push(r1->args, bilia_vector_at(t1->args, 1));
          bilia_dterm_array_push(r1->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_array_push(right->args, r0);
          bilia_dterm_array_push(right->args, r1);
          bilia_dterm_array_push(Rright->args, right);
          bilia_dterm_array_push(Rright->args, righteq);


          bilia_pure_add_dform(rcform->pure, bilia_dform_new_eq(left, Rright));
        }
      }



      break;
    }
    case BILIA_DATA_BAGUNION:
    {
      /*APP(t0 v) = max (APP(t1->0 v) APP(t1->1 v)) */
      for (uint_t j = 0; j < bilia_vector_size (lvars); j++)
      {

        bilia_var_t * var = bilia_vector_at(lvars, j);
        if (var->vty->kind == BILIA_TYP_INT )
        {

          bilia_dterm_t * left = bilia_dterm_new();
          left->kind = BILIA_DATA_APP;
          left->typ = BILIA_TYP_INT;
          left->args = bilia_dterm_array_new();
          bilia_dterm_array_push(left->args, t0);
          bilia_dterm_array_push(left->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_t * right = bilia_dterm_new();
          right->kind = BILIA_DATA_MAX;
          right->typ = BILIA_TYP_INT;
          right->args = bilia_dterm_array_new();
          bilia_dterm_t * r0 = bilia_dterm_new();
          r0->kind = BILIA_DATA_APP;
          r0->typ = BILIA_TYP_INT;
          r0->args = bilia_dterm_array_new();
          bilia_dterm_array_push(r0->args, bilia_vector_at(t1->args, 0));
          bilia_dterm_array_push(r0->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));
          bilia_dterm_t * r1 = bilia_dterm_new();
          r1->kind = BILIA_DATA_APP;
          r1->typ = BILIA_TYP_INT;
          r1->args = bilia_dterm_array_new();
          bilia_dterm_array_push(r1->args, bilia_vector_at(t1->args, 1));
          bilia_dterm_array_push(r1->args, bilia_dterm_new_var(var->vid, BILIA_TYP_INT));

          bilia_dterm_array_push(right->args, r0);
          bilia_dterm_array_push(right->args, r1);

          bilia_pure_add_dform(rcform->pure, bilia_dform_new_eq(left, right));
        }
      }



      break;
    }
    }
    /* for (uint_t j = 0; j < bilia_vector_size (dform->p.targs); j++) */
    /* { */
    /*   bilia_vector_at (dform->p.targs, j) = bilia_apply_create(bilia_vector_at (dform->p.targs, j)); */
    /* } */

  }

  bilia_entl_get()->rform = rcform;
}

void bilia_cooper()
{
  /**/

  bilia_cooper_r();
  bilia_cooper_pn(false);
  bilia_cooper_pn(true);
  /* bilia_cooper_emptybag(); */

  fprintf(stdout, "\n==================================================\n");
  bilia_entl_fprint(stdout);
  fflush(stdout);

}
