
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

#include "bilia_step2.h"
extern bilia_type_t * tyint;
bilia_type_t * tyw;
/*Here we generate W_{a,x}  variables for each int and for each bag and add it to wvars
   And add constraint such as wax >= 0 to wform*/
void create_wvars(){
        bilia_dform_array * pure = bilia_entl_get_wform();
        bilia_var_array * lvars = bilia_entl_get_var_array();
        bilia_var_array *wvars = bilia_entl_get_wvar_array();
        /*Contrainte 2-3-4*/
        for(uint_t j = 0; j<BILIA_VECTOR_SIZE(lvars); j++) {
                bilia_var_t * var_bag = bilia_vector_at(lvars,j);
                if((var_bag->vty->kind != BILIA_TYP_BAGINT) || !var_bag->used) continue;
                for(uint_t i = 0; i<BILIA_VECTOR_SIZE(lvars); i++) {
                        bilia_var_t * var_int = bilia_vector_at(lvars,i);
                        if(!var_int->element   || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                        char * v = malloc(strlen(var_bag->vname)+strlen(var_int->vname)+3);
                        sprintf(v,"w%s_%s",var_int->vname,var_bag->vname);
                        bilia_var_register (wvars, v, tyw, BILIA_SCOPE_GLOBAL,false);
                        // if(strcmp(var_bag->vname,"emptybag") == 0)
                        // {
                        //   bilia_dform_t * or = bilia_dform_new();
                        //   or->kind = BILIA_DATA_OR;
                        //   or->typ = BILIA_TYP_BOOL;
                        //   or->p.bargs = bilia_dform_array_new();
                        //   bilia_dterm_t * zero = bilia_dterm_new();
                        //   zero->kind = BILIA_DATA_INT;
                        //   zero->typ = BILIA_TYP_INT;
                        //   zero->p.value = 0;
                        //   bilia_dform_array_push(or->p.bargs,bilia_dform_new_eq(bilia_dterm_new_var(bilia_var_array_find_local(wvars,v),BILIA_TYP_W),zero));
                        //   bilia_dform_array_push(or->p.bargs,bilia_dform_new_eq(bilia_dterm_new_var(i,BILIA_TYP_INT),bilia_dterm_new_var(bot,BILIA_TYP_INT)));
                        //   bilia_dform_array_push(or->p.bargs,bilia_dform_new_eq(bilia_dterm_new_var(i,BILIA_TYP_INT),bilia_dterm_new_var(top,BILIA_TYP_INT)));
                        //   bilia_dform_array_push(pure,or);
                        //   free(v);
                        //   continue;
                        // }
                        bilia_dform_t * sup0 = bilia_dform_new();
                        if(strcmp(var_bag->vname,"emptybag") == 0)
                          sup0->kind = BILIA_DATA_EQ;
                        else
                          sup0->kind = BILIA_DATA_GE;
                        sup0->typ = BILIA_TYP_INT;
                        sup0->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(sup0->p.targs,bilia_dterm_new_var(bilia_var_array_find_local(wvars,v),BILIA_TYP_W));
                        bilia_dterm_t * zero = bilia_dterm_new();
                        zero->kind = BILIA_DATA_INT;
                        zero->typ = BILIA_TYP_INT;
                        zero->p.value = 0;
                        bilia_dterm_array_push(sup0->p.targs,zero);
                        bilia_dform_array_push(pure,sup0);
                        // if((strcmp(var_int->vname,"top") == 0||strcmp(var_int->vname,"bot") == 0 )&& strcmp(var_bag->vname,"emptybag") != 0){
                        //   bilia_dform_t * inf1 = bilia_dform_new();
                        //   inf1->kind = BILIA_DATA_LE;
                        //   inf1->typ = BILIA_TYP_INT;
                        //   inf1->p.targs = bilia_dterm_array_new();
                        //   bilia_dterm_array_push(inf1->p.targs,bilia_dterm_new_var(bilia_var_array_find_local(wvars,v),BILIA_TYP_W));
                        //   bilia_dterm_t * un = bilia_dterm_new();
                        //   un->kind = BILIA_DATA_INT;
                        //   un->typ = BILIA_TYP_INT;
                        //   un->p.value = 1;
                        //   bilia_dterm_array_push(inf1->p.targs,un);
                        //   bilia_dform_array_push(pure,inf1);
                        // }
                        free(v);
                }
        }
}
/*Here we generate W_{a,x}  constrainte such as a=b => wax = wbx
   We add them to wform*/
void constrain_wvars(){
        bilia_dform_array * pure = bilia_entl_get_wform();
        bilia_var_array *lvars = bilia_entl_get_var_array();
        bilia_var_array *wvars = bilia_entl_get_wvar_array();
        for(uint_t i = 0; i<BILIA_VECTOR_SIZE(lvars); i++) {
                bilia_var_t * var_int1 = bilia_vector_at(lvars,i);
                if(!var_int1->element  || var_int1->vty->kind != BILIA_TYP_INT  || !var_int1->used) continue;
                for(uint_t j = i+1; j<BILIA_VECTOR_SIZE(lvars); j++) {
                        bilia_var_t * var_int2 = bilia_vector_at(lvars,j);
                        if(!var_int2->element  || var_int2->vty->kind != BILIA_TYP_INT || !var_int2->used) continue;
                        for(uint_t p = 0; p<BILIA_VECTOR_SIZE(lvars); p++) {
                                bilia_var_t * var_bag = bilia_vector_at(lvars,p);
                                if(var_bag->vty->kind != BILIA_TYP_BAGINT || !var_bag->used) continue;
                                bilia_dform_t * eq = bilia_dform_new();
                                eq->kind= BILIA_DATA_NEQ;
                                eq->typ = BILIA_TYP_BOOL;
                                eq->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(eq->p.targs,bilia_dterm_new_var(var_int1->vid,BILIA_TYP_INT));
                                bilia_dterm_array_push(eq->p.targs,bilia_dterm_new_var(var_int2->vid,BILIA_TYP_INT));
                                char * v1 = malloc(strlen(var_int1->vname)+strlen(var_bag->vname)+3);
                                char * v2 = malloc(strlen(var_int2->vname)+strlen(var_bag->vname)+3);
                                sprintf(v1,"w%s_%s",var_int1->vname,var_bag->vname);
                                sprintf(v2,"w%s_%s",var_int2->vname,var_bag->vname);
                                bilia_dform_t * teq = bilia_dform_new();
                                teq->kind= BILIA_DATA_EQ;
                                teq->typ = BILIA_TYP_BOOL;
                                teq->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(teq->p.targs,bilia_dterm_new_var(bilia_var_array_find_local(wvars,v1),BILIA_TYP_W));
                                bilia_dterm_array_push(teq->p.targs,bilia_dterm_new_var(bilia_var_array_find_local(wvars,v2),BILIA_TYP_W));
                                free(v1);
                                free(v2);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->kind = BILIA_DATA_OR;
                                imp->typ = BILIA_TYP_INT;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs,eq);
                                bilia_dform_array_push(imp->p.bargs,teq);
                                bilia_dform_array_push(pure,imp);
                        }
                }
        }
}

void bilia_step2(){
        tyw = bilia_mk_type_w();
        create_wvars();
        constrain_wvars();
        /*create bot and top*/
        bilia_dform_array * pure = bilia_entl_get_wform();
        bilia_var_array * lvars = bilia_entl_get_var_array();
        bilia_var_register (lvars, "bot", tyint, BILIA_SCOPE_GLOBAL,false);
        bilia_var_register (lvars, "top", tyint, BILIA_SCOPE_GLOBAL,false);
        uint_t bot = bilia_var_array_find_local(lvars,"bot");
        uint_t top = bilia_var_array_find_local(lvars,"top");
        bilia_vector_at(lvars,bot)->used = true;
        bilia_vector_at(lvars,bot)->element = false;
        bilia_vector_at(lvars,top)->used = true;
        bilia_vector_at(lvars,top)->element = false;
        for(uint_t i = 0; i<BILIA_VECTOR_SIZE(lvars); i++) {
          bilia_var_t * var_int = bilia_vector_at(lvars,i);
          if(!var_int->element || var_int->vty->kind != BILIA_TYP_INT || !var_int->used || i == bot || i == top) continue;
          bilia_dform_t * inf = bilia_dform_new();
          if(var_int->extremum)
            inf->kind = BILIA_DATA_LE;
          else
            inf->kind = BILIA_DATA_LT;
          inf->typ = BILIA_TYP_INT;
          inf->p.targs = bilia_dterm_array_new();
          bilia_dterm_array_push(inf->p.targs,bilia_dterm_new_var(bot,BILIA_TYP_INT));
          bilia_dterm_t * un = bilia_dterm_new_var(i,BILIA_TYP_INT);
          bilia_dterm_array_push(inf->p.targs,un);
          bilia_dform_array_push(pure,inf);
          bilia_dform_t * sup = bilia_dform_new();
          if(var_int->extremum)
            sup->kind = BILIA_DATA_GE;
          else
            sup->kind = BILIA_DATA_GT;
          sup->typ = BILIA_TYP_INT;
          sup->p.targs = bilia_dterm_array_new();
          bilia_dterm_array_push(sup->p.targs,bilia_dterm_new_var(top,BILIA_TYP_INT));
          bilia_dterm_t * deux = bilia_dterm_new_var(i,BILIA_TYP_INT);
          bilia_dterm_array_push(sup->p.targs,deux);
          bilia_dform_array_push(pure,sup);
        }
        if (bilia_option_get_verb () > 0) {
                fprintf(stdout, "\n=================================================\n");
                bilia_entl_fprint(stdout);
                fflush(stdout);
        }
}
