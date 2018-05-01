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
 * VAR_CHECK.
 */

#include "bilia_var_check.h"

bool bilia_term_is_element(bilia_dterm_t *term){
  bool res = false;
  if(term->args != NULL)
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(term->args); i++) {
      res = res || bilia_term_is_element(bilia_vector_at(term->args,i));
    }
  else if(term->kind == BILIA_DATA_VAR){
      bilia_var_t * var = bilia_vector_at(bilia_entl_get_var_array(),term->p.sid);
      res = res || var->element;
  }
  return res;
}

void bilia_var_used_term(bilia_dterm_t *term,bool element){
  if(term->args != NULL){
    bool element2 = element;
    if(term->kind == BILIA_DATA_BAG || term->kind == BILIA_DATA_BAGN)
      element2 = true;
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(term->args); i++)
      bilia_var_used_term(bilia_vector_at(term->args,i),element2);
  }
  if(term->kind == BILIA_DATA_VAR){
      bilia_var_t * var = bilia_vector_at(bilia_entl_get_var_array(),term->p.sid);
      //printf("===>%s\n",var->vname);
      var->used = true;
      if(element && var->vty->kind == BILIA_TYP_INT)var->element = true;
  }
}

void bilia_var_used(bilia_dform_t * form){
  if(form->kind == BILIA_DATA_AND || form->kind == BILIA_DATA_OR || form->kind == BILIA_DATA_IMPLIES){
    for (uint_t i = 0;  i< BILIA_VECTOR_SIZE(form->p.bargs); i++)
      bilia_var_used(bilia_vector_at(form->p.bargs,i));
    return;
  }
  bool element = false;
  switch(form->kind){
    case BILIA_DATA_SUBSETEQ:
    case BILIA_DATA_NSUBSETEQ:
    case BILIA_DATA_IN:
    case BILIA_DATA_NI:
    case BILIA_DATA_INN:
    case BILIA_DATA_NIN:
      element = true;
      break;
    }
  if(form->kind == BILIA_DATA_EQ || form->kind == BILIA_DATA_NEQ || form->kind == BILIA_DATA_LE || form->kind == BILIA_DATA_GE || form->kind == BILIA_DATA_GT \
   || form->kind == BILIA_DATA_LT || form->kind == BILIA_DATA_INN  || form->kind == BILIA_DATA_NIN  || form->kind == BILIA_DATA_IN  || form->kind == BILIA_DATA_NI \
    || form->kind == BILIA_DATA_SUBSETEQ || form->kind == BILIA_DATA_NSUBSETEQ)
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.targs) ; i++)
      bilia_var_used_term(bilia_vector_at(form->p.targs,i),element);
}

int bilia_term_apply_element(bilia_dterm_t * term){
  int res = 0;
  if(term->args != NULL)
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(term->args); i++)
      res += bilia_term_apply_element(bilia_vector_at(term->args,i));
  else if(term->kind == BILIA_DATA_VAR){
      bilia_var_t * var = bilia_vector_at(bilia_entl_get_var_array(),term->p.sid);
      if(var->vty->kind == BILIA_TYP_INT && !var->element){
        var->element = true;
        res ++;
      }
  }
  return res;
}

int bilia_var_element(bilia_dform_t * form){
  int nb_switch = 0;
  if(form->kind == BILIA_DATA_AND || form->kind == BILIA_DATA_OR){
    /*Propagate the nnf*/
    for (uint_t i = 0;  i< BILIA_VECTOR_SIZE(form->p.bargs); i++)
      nb_switch += bilia_var_element(bilia_vector_at(form->p.bargs,i));
    return nb_switch;
  }
  if(form->kind == BILIA_DATA_EQ || form->kind == BILIA_DATA_NEQ || form->kind == BILIA_DATA_LE || form->kind == BILIA_DATA_GE || form->kind == BILIA_DATA_GT \
   || form->kind == BILIA_DATA_LT || form->kind == BILIA_DATA_INN  || form->kind == BILIA_DATA_NIN  || form->kind == BILIA_DATA_IN  || form->kind == BILIA_DATA_NI \
    || form->kind == BILIA_DATA_SUBSETEQ || form->kind == BILIA_DATA_NSUBSETEQ){
    /*Here we try to know if we have to deal with ite*/
    bool is_element = false;
    for (uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.targs) ; i++)
      is_element = is_element || bilia_term_is_element(bilia_vector_at(form->p.targs,i));
    if(is_element)
      for (uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.targs) ; i++)
        nb_switch += bilia_term_apply_element(bilia_vector_at(form->p.targs,i));
   }
  return nb_switch;
}

void bilia_var_union_find(bilia_dform_t * form){
  /*Not yet a union find ... BRUTE FORCE for now*/
  while(bilia_var_element(form)){};
}

void bilia_var_check(){
  /*Premier parcours pour les useds variable et les elements variable*/
  bilia_dform_array * rf = bilia_entl_get_rform();
  for (size_t i = 0; i < BILIA_VECTOR_SIZE(rf); i++)
    bilia_var_used(bilia_vector_at(rf,i));
  bilia_dform_array * af = bilia_entl_get_nform();
  for (size_t i = 0; i < BILIA_VECTOR_SIZE(af); i++)
    bilia_var_used(bilia_vector_at(af,i));
  if(bilia_option_get_optim_ele()){
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(rf); i++)
      bilia_var_union_find(bilia_vector_at(rf,i));
    for (size_t i = 0; i < BILIA_VECTOR_SIZE(af); i++)
      bilia_var_union_find(bilia_vector_at(af,i));
  }
}
