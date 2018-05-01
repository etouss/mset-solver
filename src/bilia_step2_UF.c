

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
 * Step2
 */

#include "bilia_step2_UF.h"
extern bilia_type_t * tyint;

/*here we constraint our UF such as x(a) >= 0 for all a and x
We dont change x type to uf as we don't do type check anymore and it is just a declaration we will modify when writting the output file*/
void constrain_uf(){
  bilia_dform_array * pure = bilia_entl_get_wform();
  bilia_var_array *lvars = bilia_entl_get_var_array();
  for(uint_t i = 0; i<BILIA_VECTOR_SIZE(lvars);i++){
     bilia_var_t * var_int1 = bilia_vector_at(lvars,i);
     if((!var_int1->element && bilia_option_get_optim_ele()) || var_int1->vty->kind != BILIA_TYP_INT || !var_int1->used)continue;
       for(uint_t p = 0;p<BILIA_VECTOR_SIZE(lvars);p++){
         bilia_var_t * var_bag = bilia_vector_at(lvars,p);
         if(var_bag->vty->kind != BILIA_TYP_BAGINT || !var_bag->used)continue;
         bilia_dterm_t * zero = bilia_dterm_new();
         zero->kind = BILIA_DATA_INT;
         zero->typ = BILIA_TYP_INT;
         zero->p.value = 0;
         bilia_dform_t * geq = bilia_dform_new();
         if(strcmp(var_bag->vname,"emptybag") == 0)
          geq->kind = BILIA_DATA_EQ;
         else
          geq->kind = BILIA_DATA_GE;
         geq->typ = BILIA_TYP_INT;
         geq->p.targs = bilia_dterm_array_new();
         bilia_dterm_t * app = bilia_dterm_new();
         app->kind = BILIA_DATA_APP;
         app->typ = BILIA_TYP_INT;
         app->args = bilia_dterm_array_new();
         bilia_dterm_array_push(app->args,bilia_dterm_new_var(var_bag->vid,BILIA_TYP_BAGINT));
         bilia_dterm_array_push(app->args,bilia_dterm_new_var(var_int1->vid,BILIA_TYP_INT));
         bilia_dterm_array_push(geq->p.targs,app);
         bilia_dterm_array_push(geq->p.targs,zero);
         bilia_dform_array_push(pure,geq);
     }
  }
}

void bilia_step2_uf(){
  constrain_uf();
  if (bilia_option_get_verb () > 0){
  fprintf(stdout, "\n=================================================\n");
  bilia_entl_fprint(stdout);
  fflush(stdout);
  }
}
