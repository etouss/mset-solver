

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
 *
 */


/* A modifier c'est laid et c'est plus pour les tests*/
#include "bilia2smt.h"

void bilia2smt()
{
  if (!bilia_option_get_uinterpreted())
  {
    char * name = malloc(strlen("../smt/")+strlen(file_name)+1);
    sprintf(name,"../smt/%s",file_name);
    FILE *f = fopen (name, "w");
    free(name);
    fprintf(f, "(set-logic QF_LIA)\n");
    fprintf(f, "(define-fun max ((x Int) (y Int)) Int (ite (< x y) y x))\n");
    fprintf(f, "(define-fun min ((x Int) (y Int)) Int (ite (> x y) y x))\n");

    bilia_var_array_fprint (f, bilia_prob->lvars, "lvars", false);
    bilia_var_array_fprint (f, bilia_prob->wvars, "lvars", false);
    // fprintf(f, "\n(assert (and ");
    // bilia_dform_array_fprint (f,  bilia_prob->lvars,bilia_prob->wform,0);
    // fprintf(f, "))");
    // fprintf(f, "\n(assert (and");
    // bilia_dform_array_fprint (f,  bilia_prob->lvars,bilia_prob->rform,0);
    // fprintf(f, "))");
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->nform); i++) {
      fprintf(f, "\n(assert ");
      bilia_dform_fprint (f,  bilia_prob->lvars,bilia_vector_at(bilia_prob->nform,i),0);
      fprintf(f, ")");
    }
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->rform); i++) {
      fprintf(f, "\n(assert ");
      bilia_dform_fprint (f,  bilia_prob->lvars,bilia_vector_at(bilia_prob->rform,i),0);
      fprintf(f, ")");
    }
    fprintf(f, "\n(assert (and ");
    bilia_dform_array_fprint (f,  bilia_prob->lvars,bilia_prob->wform,0);
    fprintf(f, "))");
    fprintf(f, "\n(check-sat)");
    //fprintf(f, "\n(get-model)");
    //fprintf(f, "\n(get-info :all-statistics)");
    fflush (f);
    fclose(f);
  }
  else{
    char * name = malloc(strlen("../smt/")+strlen(file_name)+1);
    sprintf(name,"../smt/%s",file_name);
    FILE *f = fopen (name, "w");
    free(name);
    fprintf(f, "(set-logic QF_UFLIA)\n");
    fprintf(f, "(define-fun max ((x Int) (y Int)) Int (ite (< x y) y x))\n");
    fprintf(f, "(define-fun min ((x Int) (y Int)) Int (ite (> x y) y x))\n");

    bilia_var_array_fprint (f, bilia_prob->lvars, "lvars", true);
    bilia_var_array_fprint (f, bilia_prob->wvars, "lvars", true);
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->nform); i++) {
      fprintf(f, "\n(assert ");
      bilia_dform_fprint (f,  bilia_prob->lvars,bilia_vector_at(bilia_prob->nform,i),0);
      fprintf(f, ")");
    }
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_prob->rform); i++) {
      fprintf(f, "\n(assert ");
      bilia_dform_fprint (f,  bilia_prob->lvars,bilia_vector_at(bilia_prob->rform,i),0);
      fprintf(f, ")");
    }
    fprintf(f, "\n(assert (and ");
    bilia_dform_array_fprint (f,  bilia_prob->lvars,bilia_prob->wform,0);
    fprintf(f, "))");
    fprintf(f, "\n(check-sat)");
    //fprintf(f, "\n(get-model)");
    //fprintf(f, "\n(get-info :all-statistics)");
    fflush (f);
    fclose(f);
  }

}
