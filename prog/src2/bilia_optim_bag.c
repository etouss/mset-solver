
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
 * STEP 0-1.
 */

#include "bilia_optim_bag.h"

/*In this fonction we want to avoid creating bag and bagn as much as possible*/
void bilia_optim_bag_aux(bilia_dform_array* df)
{

  for (uint_t i = 0; i < bilia_vector_size (df); i++)
  {
    bilia_dform_t * dform = bilia_vector_at(df, i);
    switch (dform->kind)
    {
    case BILIA_DATA_AND:
    case BILIA_DATA_OR:
      {
          bilia_optim_bag_aux(dform->p.bargs);
      }
    case BILIA_DATA_SUBSETEQ:
    /*a subset x   == a in x*/
      if (bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAG)
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 0);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
        dform->p.targs = bilia_dterm_array_new();
        dform->kind = BILIA_DATA_IN;
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
      }
      else if (bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAGN)
      {
        bilia_dterm_t * bagn =  bilia_vector_at(dform->p.targs, 0);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
        dform->p.targs = bilia_dterm_array_new();
        dform->kind = BILIA_DATA_INN;
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 0)));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 1)));

      }
      else if (bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAG)
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 1);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
        dform->p.targs = bilia_dterm_array_new();
        dform->kind = BILIA_DATA_IN;
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
      }
      else if (bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAGN)
      {
        bilia_dterm_t * bagn =  bilia_vector_at(dform->p.targs, 1);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
        dform->p.targs = bilia_dterm_array_new();
        dform->kind = BILIA_DATA_IN;
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 0)));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 1)));
      }
      break;
      case BILIA_DATA_NSUBSETEQ:
      /*a subset x   == a innot x*/
        if (bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAG)
        {
          bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 0);
          bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
          dform->p.targs = bilia_dterm_array_new();
          dform->kind = BILIA_DATA_NI;
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
        }
        else if (bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAGN)
        {
          bilia_dterm_t * bagn =  bilia_vector_at(dform->p.targs, 0);
          bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
          dform->p.targs = bilia_dterm_array_new();
          dform->kind = BILIA_DATA_NIN;
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 0)));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 1)));

        }
        else if (bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAG)
        {
          bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 1);
          bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
          dform->p.targs = bilia_dterm_array_new();
          dform->kind = BILIA_DATA_NI;
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
        }
        else if (bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAGN)
        {
          bilia_dterm_t * bagn =  bilia_vector_at(dform->p.targs, 1);
          bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
          dform->p.targs = bilia_dterm_array_new();
          dform->kind = BILIA_DATA_NIN;
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 0)));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(rec));
          bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bagn->args, 1)));
        }
        break;
    case BILIA_DATA_LE:
    case BILIA_DATA_LT:
    /*a < x   == a < minx*/
      if ((bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAG) || bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAGN)
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 0);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
        dform->p.targs = bilia_dterm_array_new();
        bilia_dterm_t * nrec = bilia_dterm_new();
        nrec->kind = BILIA_DATA_BAGMIN;
        nrec->typ = BILIA_TYP_INT;
        nrec->args = bilia_dterm_array_new();
        bilia_dterm_array_push(nrec->args, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, bilia_vector_at(bag->args, 0));
        bilia_dterm_array_push(dform->p.targs, nrec);
      }
      else if ((bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAG)||(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAGN))
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 1);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
        dform->p.targs = bilia_dterm_array_new();
        bilia_dterm_t * nrec = bilia_dterm_new();
        nrec->kind = BILIA_DATA_BAGMAX;
        nrec->typ = BILIA_TYP_INT;
        nrec->args = bilia_dterm_array_new();
        bilia_dterm_array_push(nrec->args, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, nrec);
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
      }
      break;
    case BILIA_DATA_GE:
    case BILIA_DATA_GT:
    /*a > x   == a > maxx*/
      if ((bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAG)||(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_BAGN))
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 0);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 1);
        dform->p.targs = bilia_dterm_array_new();
        bilia_dterm_t * nrec = bilia_dterm_new();
        nrec->kind = BILIA_DATA_BAGMAX;
        nrec->typ = BILIA_TYP_INT;
        nrec->args = bilia_dterm_array_new();
        bilia_dterm_array_push(nrec->args, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
        bilia_dterm_array_push(dform->p.targs, nrec);
      }
      else if ((bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAG)||(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_BAGN))
      {
        bilia_dterm_t * bag =  bilia_vector_at(dform->p.targs, 1);
        bilia_dterm_t * rec = bilia_vector_at(dform->p.targs, 0);
        dform->p.targs = bilia_dterm_array_new();
        bilia_dterm_t * nrec = bilia_dterm_new();
        nrec->kind = BILIA_DATA_BAGMIN;
        nrec->typ = BILIA_TYP_INT;
        nrec->args = bilia_dterm_array_new();
        bilia_dterm_array_push(nrec->args, bilia_dterm_copy(rec));
        bilia_dterm_array_push(dform->p.targs, nrec);
        bilia_dterm_array_push(dform->p.targs, bilia_dterm_copy(bilia_vector_at(bag->args, 0)));
      }
      break;
    }
  }
}

void bilia_optim_bag()
{
  bilia_optim_bag_aux(bilia_entl_get_nform());
  /* fprintf(stdout, "\n=================================================\n"); */
  /* bilia_entl_fprint(stdout); */
  /* fflush(stdout); */
}
