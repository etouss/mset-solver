

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
 * Step3
 */

#include "bilia_step3.h"

void breakpoints3(){
}

/*Return the correspond w_varint_varbag dterm*/
bilia_dterm_t * matching_w(char * var_int, char * var_bag)
{
        char * v = malloc(strlen(var_int) + strlen(var_bag) + 3);
        sprintf(v, "w%s_%s", var_int, var_bag);
        bilia_dterm_t * res = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_wvar_array(), v), BILIA_TYP_W);
        free(v);
        return res;
}

bilia_dterm_t * construct(char * var_int,bilia_dterm_array * array,int debut,bilia_data_op_t opt){
        bilia_data_op_t op = opt;
        switch(opt) {
        case BILIA_DATA_BAGUNION:
                op = BILIA_DATA_MAX;
                break;
        case BILIA_DATA_BAGSUM:
                op = BILIA_DATA_PLUS;
                break;
        case BILIA_DATA_BAGINTER:
                op = BILIA_DATA_MIN;
                break;
        }
        bilia_dterm_t * debut_term = bilia_vector_at(array,debut);
        if(debut_term->kind != BILIA_DATA_VAR)
                debut_term = construct(var_int,debut_term->args,0,debut_term->kind);
        else
                debut_term = matching_w(var_int, bilia_vector_at(bilia_entl_get_var_array(),debut_term->p.sid)->vname);
        if(debut == BILIA_VECTOR_SIZE(array)-1)
                return debut_term;
        bilia_dterm_t * max = bilia_dterm_new();
        max->kind = op;
        max->typ = BILIA_TYP_INT;
        max->args = bilia_dterm_array_new();
        bilia_dterm_array_push(max->args, debut_term);
        bilia_dterm_array_push(max->args, construct(var_int,array,debut+1,op));
        return max;
}

/*Apply small model rewritting to every litterals*/
void new_constraint(bilia_dform_t * f,bilia_dform_t ** replace)
{
        bilia_var_array *  lvars  = bilia_entl_get_var_array();
        /* bilia_dform_t * f; */
        switch (f->kind)
        {
        case BILIA_DATA_AND:
        case BILIA_DATA_OR:
        {
                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(f->p.bargs); i++)
                        new_constraint(bilia_vector_at(f->p.bargs,i),&bilia_vector_at(f->p.bargs,i));
        }
        break;
        case BILIA_DATA_EQ:
        {
                bilia_dterm_t * tl = bilia_vector_at(f->p.targs, 0);
                bilia_dterm_t * tr = bilia_vector_at(f->p.targs, 1);
                switch (tr->kind)
                {
                case BILIA_DATA_VAR:
                {
                        if (tr->typ == BILIA_TYP_INT) return;
                        else
                        {
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * var_bag_r = bilia_vector_at(lvars, tr->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), matching_w(var_int->vname, var_bag_r->vname)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                        }
                        break;
                }
                case BILIA_DATA_BAGUNION:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_MAX;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b0->vname));
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b1->vname));
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                // bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                // bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), construct(var_int->vname,tr->args,0,BILIA_DATA_MAX)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }
                case BILIA_DATA_BAGINTER:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_MIN;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b0->vname));
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b1->vname));
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                // bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                // bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), construct(var_int->vname,tr->args,0,BILIA_DATA_MIN)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }
                case BILIA_DATA_BAGSUM:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_PLUS;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b0->vname));
                                        bilia_dterm_array_push(max->args, matching_w(var_int->vname, b1->vname));
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                                // bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                                // bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), construct(var_int->vname,tr->args,0,BILIA_DATA_PLUS)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }
                case BILIA_DATA_BAGMINUS:
                {
                        bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                        bilia_var_t * b0 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 0)->p.sid);
                        bilia_var_t * b1 = bilia_vector_at(lvars, bilia_vector_at(tr->args, 1)->p.sid);
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * minus = bilia_dterm_new();
                                minus->kind = BILIA_DATA_MINUS;
                                minus->typ = BILIA_TYP_INT;
                                minus->args = bilia_dterm_array_new();
                                bilia_dterm_array_push(minus->args, matching_w(var_int->vname, b0->vname));
                                bilia_dterm_array_push(minus->args, matching_w(var_int->vname, b1->vname));
                                bilia_dterm_t * max = bilia_dterm_new();
                                max->kind = BILIA_DATA_MAX;
                                max->typ = BILIA_TYP_INT;
                                max->args = bilia_dterm_array_new();
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dterm_array_push(max->args, minus);
                                bilia_dterm_array_push(max->args, zero);
                                bilia_dform_array_push(and->p.bargs,bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), max));
                        }
                        bilia_dform_free(*replace);
                        *replace = and;
                        break;
                }
                case BILIA_DATA_BAG:
                {
                        bilia_dterm_t * ta0 =  bilia_vector_at(tr->args, 0);
                        bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                        bilia_var_t * a0 = bilia_vector_at(lvars, ta0->p.sid);
                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = 1;
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(matching_w(a0->vname, var_bag_l->vname), un));

                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vid == a0->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_NEQ;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(ta0));
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                bilia_dform_t * impr = bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                        }
                        bilia_dform_free(*replace);
                        *replace = and;
                        break;
                }
                case BILIA_DATA_BAGN:
                {
                        bilia_dterm_t * ta0 =  bilia_vector_at(tr->args, 0);
                        long n  =  bilia_vector_at(tr->args, 1)->p.value;
                        bilia_var_t * var_bag_l = bilia_vector_at(lvars, tl->p.sid);
                        bilia_var_t * a0 = bilia_vector_at(lvars, ta0->p.sid);
                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = n;
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(matching_w(a0->vname, var_bag_l->vname), un));

                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vid == a0->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_NEQ;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(ta0));
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                bilia_dform_t * impr = bilia_dform_new_eq(matching_w(var_int->vname, var_bag_l->vname), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                        }
                        bilia_dform_free(*replace);
                        *replace = and;
                        break;
                }
                case BILIA_DATA_BAGMIN:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                                bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);

                                bilia_dterm_t * un = bilia_dterm_new();
                                un->kind = BILIA_DATA_INT;
                                un->typ = BILIA_TYP_INT;
                                un->p.value = 1;

                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();

                                bilia_dform_t * geq = bilia_dform_new();
                                geq->typ = BILIA_TYP_INT;
                                geq->kind = BILIA_DATA_GE;
                                geq->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(geq->p.targs, matching_w(var_int_l->vname, var_bag_in->vname));
                                bilia_dterm_array_push(geq->p.targs, un);
                                bilia_dform_array_push(and->p.bargs, geq);

                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * zero = bilia_dterm_new();
                                        zero->kind = BILIA_DATA_INT;
                                        zero->typ = BILIA_TYP_INT;
                                        zero->p.value = 0;
                                        bilia_dform_t * impl = bilia_dform_new();
                                        impl->kind = BILIA_DATA_LT;
                                        impl->typ = BILIA_TYP_INT;
                                        impl->p.targs = bilia_dterm_array_new();
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                        bilia_dform_t * impr = bilia_dform_new_eq(matching_w(var_int->vname, var_bag_in->vname), zero);
                                        bilia_dform_t * imp = bilia_dform_new();
                                        imp->typ = BILIA_TYP_INT;
                                        imp->kind = BILIA_DATA_IMPLIES;
                                        imp->p.bargs = bilia_dform_array_new();
                                        bilia_dform_array_push(imp->p.bargs, impl);
                                        bilia_dform_array_push(imp->p.bargs, impr);
                                        bilia_dform_array_push(and->p.bargs, imp);
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_dterm_t * bags =  bilia_vector_at(tr->args, 0);
                                bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                                //bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);

                                bilia_dterm_t * un = bilia_dterm_new();
                                un->kind = BILIA_DATA_INT;
                                un->typ = BILIA_TYP_INT;
                                un->p.value = 1;

                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();

                                bilia_dform_t * geq = bilia_dform_new();
                                geq->typ = BILIA_TYP_INT;
                                geq->kind = BILIA_DATA_GE;
                                geq->p.targs = bilia_dterm_array_new();
                                if(bags->kind == BILIA_DATA_VAR)
                                        bilia_dterm_array_push(geq->p.targs, matching_w(var_int_l->vname, bilia_vector_at(lvars, bags->p.sid)->vname));
                                else
                                        bilia_dterm_array_push(geq->p.targs, construct(var_int_l->vname,bags->args,0,bags->kind));
                                bilia_dterm_array_push(geq->p.targs, un);
                                bilia_dform_array_push(and->p.bargs, geq);

                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * zero = bilia_dterm_new();
                                        zero->kind = BILIA_DATA_INT;
                                        zero->typ = BILIA_TYP_INT;
                                        zero->p.value = 0;
                                        bilia_dform_t * impl = bilia_dform_new();
                                        impl->kind = BILIA_DATA_LT;
                                        impl->typ = BILIA_TYP_INT;
                                        impl->p.targs = bilia_dterm_array_new();
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                        bilia_dform_t * impr;
                                        if(bags->kind == BILIA_DATA_VAR)
                                                impr = bilia_dform_new_eq(matching_w(var_int->vname, bilia_vector_at(lvars, bags->p.sid)->vname), zero);
                                        else
                                                impr = bilia_dform_new_eq(construct(var_int->vname,bags->args,0,bags->kind), zero);
                                        bilia_dform_t * imp = bilia_dform_new();
                                        imp->typ = BILIA_TYP_INT;
                                        imp->kind = BILIA_DATA_IMPLIES;
                                        imp->p.bargs = bilia_dform_array_new();
                                        bilia_dform_array_push(imp->p.bargs, impl);
                                        bilia_dform_array_push(imp->p.bargs, impr);
                                        bilia_dform_array_push(and->p.bargs, imp);
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }
                case BILIA_DATA_BAGMAX:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                                bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                                bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);
                                bilia_dterm_t * un = bilia_dterm_new();
                                un->kind = BILIA_DATA_INT;
                                un->typ = BILIA_TYP_INT;
                                un->p.value = 1;

                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();

                                bilia_dform_t * geq = bilia_dform_new();
                                geq->typ = BILIA_TYP_INT;
                                geq->kind = BILIA_DATA_GE;
                                geq->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(geq->p.targs, matching_w(var_int_l->vname, var_bag_in->vname));
                                bilia_dterm_array_push(geq->p.targs, un);
                                bilia_dform_array_push(and->p.bargs, geq);

                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * zero = bilia_dterm_new();
                                        zero->kind = BILIA_DATA_INT;
                                        zero->typ = BILIA_TYP_INT;
                                        zero->p.value = 0;
                                        bilia_dform_t * impl = bilia_dform_new();
                                        impl->kind = BILIA_DATA_GT;
                                        impl->typ = BILIA_TYP_INT;
                                        impl->p.targs = bilia_dterm_array_new();
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                        bilia_dform_t * impr = bilia_dform_new_eq(matching_w(var_int->vname, var_bag_in->vname), zero);
                                        bilia_dform_t * imp = bilia_dform_new();
                                        imp->typ = BILIA_TYP_INT;
                                        imp->kind = BILIA_DATA_IMPLIES;
                                        imp->p.bargs = bilia_dform_array_new();
                                        bilia_dform_array_push(imp->p.bargs, impl);
                                        bilia_dform_array_push(imp->p.bargs, impr);
                                        bilia_dform_array_push(and->p.bargs, imp);
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_dterm_t * bags =  bilia_vector_at(tr->args, 0);
                                bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                                //bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);

                                bilia_dterm_t * un = bilia_dterm_new();
                                un->kind = BILIA_DATA_INT;
                                un->typ = BILIA_TYP_INT;
                                un->p.value = 1;

                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();

                                bilia_dform_t * geq = bilia_dform_new();
                                geq->typ = BILIA_TYP_INT;
                                geq->kind = BILIA_DATA_GE;
                                geq->p.targs = bilia_dterm_array_new();
                                if(bags->kind == BILIA_DATA_VAR)
                                        bilia_dterm_array_push(geq->p.targs, matching_w(var_int_l->vname, bilia_vector_at(lvars, bags->p.sid)->vname));
                                else
                                        bilia_dterm_array_push(geq->p.targs, construct(var_int_l->vname,bags->args,0,bags->kind));
                                bilia_dterm_array_push(geq->p.targs, un);
                                bilia_dform_array_push(and->p.bargs, geq);

                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * zero = bilia_dterm_new();
                                        zero->kind = BILIA_DATA_INT;
                                        zero->typ = BILIA_TYP_INT;
                                        zero->p.value = 0;
                                        bilia_dform_t * impl = bilia_dform_new();
                                        impl->kind = BILIA_DATA_GT;
                                        impl->typ = BILIA_TYP_INT;
                                        impl->p.targs = bilia_dterm_array_new();
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                        bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                        bilia_dform_t * impr;
                                        if(bags->kind == BILIA_DATA_VAR)
                                                impr = bilia_dform_new_eq(matching_w(var_int->vname, bilia_vector_at(lvars, bags->p.sid)->vname), zero);
                                        else
                                                impr = bilia_dform_new_eq(construct(var_int->vname,bags->args,0,bags->kind), zero);
                                        bilia_dform_t * imp = bilia_dform_new();
                                        imp->typ = BILIA_TYP_INT;
                                        imp->kind = BILIA_DATA_IMPLIES;
                                        imp->p.bargs = bilia_dform_array_new();
                                        bilia_dform_array_push(imp->p.bargs, impl);
                                        bilia_dform_array_push(imp->p.bargs, impr);
                                        bilia_dform_array_push(and->p.bargs, imp);
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;

                        }
                }
                case BILIA_DATA_EMPTYBAG:
                {
                        bilia_var_t * var_bag = bilia_vector_at(lvars, tl->p.sid);
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();

                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(matching_w(var_int->vname, var_bag->vname), zero));
                        }
                        bilia_dform_free(*replace);
                        *replace = and;
                        break;
                }
                default:
                        break;
                }
                break;
        }
        case BILIA_DATA_SUBSETEQ:
        {
                bilia_dform_t * and = bilia_dform_new();
                and->typ = BILIA_TYP_BOOL;
                and->kind = BILIA_DATA_AND;
                and->p.bargs = bilia_dform_array_new();

                bilia_var_t * bag_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                {
                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                        bilia_dform_t * leq = bilia_dform_new();
                        leq->kind = BILIA_DATA_LE;
                        leq->typ = BILIA_TYP_INT;
                        leq->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(leq->p.targs, matching_w(var_int->vname, bag_l->vname));
                        bilia_dterm_array_push(leq->p.targs, matching_w(var_int->vname, bag_r->vname));
                        bilia_dform_array_push(and->p.bargs, leq);
                }
                bilia_dform_free(*replace);
                *replace = and;
                break;
        }
        case BILIA_DATA_NEQ:
        {
                if (bilia_vector_at(f->p.targs, 0)->typ != BILIA_TYP_BAGINT)
                        break;
                //construct(var_int->vname,tr->args,0,BILIA_DATA_MIN)
                if(!bilia_option_get_nr()) {
                        bilia_var_t * bag_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                        bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                        bilia_dform_t * distinct = bilia_dform_new();
                        distinct->kind = BILIA_DATA_NEQ;
                        distinct->typ = BILIA_TYP_INT;
                        distinct->p.targs = bilia_dterm_array_new();
                        char * v = malloc(strlen(bag_l->vname) + strlen(bag_r->vname) + 4);
                        sprintf(v, "%sneq%s", bag_l->vname, bag_r->vname);
                        if (bilia_var_array_find_local(lvars, v) == UNDEFINED_ID)
                                sprintf(v, "%sneq%s", bag_r->vname, bag_l->vname);
                        bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_l->vname));
                        bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_r->vname));
                        bilia_dform_free(*replace);
                        *replace = distinct;
                        free(v);
                }
                else{
                        bilia_dform_t * distinct = bilia_dform_new();
                        distinct->kind = BILIA_DATA_NEQ;
                        distinct->typ = BILIA_TYP_INT;
                        distinct->p.targs = bilia_dterm_array_new();
                        bilia_dterm_t * bag_l = bilia_vector_at(f->p.targs, 0);
                        bilia_dterm_t * bag_r = bilia_vector_at(f->p.targs, 1);
                        char * bag_l_name;
                        char * bag_r_name;
                        if(bag_l->kind == BILIA_DATA_VAR)
                                bag_l_name = bilia_vector_at(lvars, bag_l->p.sid)->vname;
                        else
                                bag_l_name = create_var_from_dterm(bag_l->args,bag_l->kind);
                        if(bag_r->kind == BILIA_DATA_VAR)
                                bag_r_name = bilia_vector_at(lvars, bag_r->p.sid)->vname;
                        else
                                bag_r_name = create_var_from_dterm(bag_r->args,bag_r->kind);
                        char * v = malloc(strlen(bag_l_name) + strlen(bag_r_name) + 4);
                        sprintf(v, "%sneq%s", bag_l_name, bag_r_name);
                        if (bilia_var_array_find_local(lvars, v) == UNDEFINED_ID)
                                sprintf(v, "%sneq%s", bag_r_name, bag_l_name);
                        if(bag_l->kind == BILIA_DATA_VAR)
                                bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_l_name));
                        else{
                                bilia_dterm_array_push(distinct->p.targs, construct(v,bag_l->args,0,bag_l->kind));
                                free(bag_l_name);
                        }
                        if(bag_r->kind == BILIA_DATA_VAR)
                                bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_r_name));
                        else{
                                bilia_dterm_array_push(distinct->p.targs, construct(v,bag_r->args,0,bag_r->kind));
                                free(bag_r_name);
                        }
                        bilia_dform_free(*replace);
                        *replace = distinct;
                        free(v);

                }
                break;
        }
        case BILIA_DATA_NSUBSETEQ:
        {
                if(!bilia_option_get_nr()) {
                        bilia_var_t * bag_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                        bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                        bilia_dform_t * distinct = bilia_dform_new();
                        distinct->kind = BILIA_DATA_GT;
                        distinct->typ = BILIA_TYP_INT;
                        distinct->p.targs = bilia_dterm_array_new();
                        char * v = malloc(strlen(bag_l->vname) + strlen(bag_r->vname) + 10);
                        sprintf(v, "%snsubseteq%s", bag_l->vname, bag_r->vname);
                        bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_l->vname));
                        bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_r->vname));
                        bilia_dform_free(*replace);
                        *replace = distinct;
                        free(v);
                }
                else{
                        bilia_dform_t * distinct = bilia_dform_new();
                        distinct->kind = BILIA_DATA_GT;
                        distinct->typ = BILIA_TYP_INT;
                        distinct->p.targs = bilia_dterm_array_new();
                        bilia_dterm_t * bag_l = bilia_vector_at(f->p.targs, 0);
                        bilia_dterm_t * bag_r = bilia_vector_at(f->p.targs, 1);
                        char * bag_l_name;
                        char * bag_r_name;
                        if(bag_l->kind == BILIA_DATA_VAR)
                                bag_l_name = bilia_vector_at(lvars, bag_l->p.sid)->vname;
                        else
                                bag_l_name = create_var_from_dterm(bag_l->args,bag_l->kind);
                        if(bag_r->kind == BILIA_DATA_VAR)
                                bag_r_name = bilia_vector_at(lvars, bag_r->p.sid)->vname;
                        else
                                bag_r_name = create_var_from_dterm(bag_r->args,bag_r->kind);
                        char * v = malloc(strlen(bag_l_name) + strlen(bag_r_name) + strlen("nsubseteq") + 1);
                        sprintf(v, "%snsubseteq%s", bag_l_name, bag_r_name);
                        printf(":::::%s\n",v);
                        if(bag_l->kind == BILIA_DATA_VAR)
                                bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_l_name));
                        else{
                                bilia_dterm_array_push(distinct->p.targs, construct(v,bag_l->args,0,bag_l->kind));
                                free(bag_l_name);
                        }
                        if(bag_r->kind == BILIA_DATA_VAR)
                                bilia_dterm_array_push(distinct->p.targs, matching_w(v, bag_r_name));
                        else{
                                bilia_dterm_array_push(distinct->p.targs, construct(v,bag_r->args,0,bag_r->kind));
                                free(bag_r_name);
                        }
                        bilia_dform_free(*replace);
                        *replace = distinct;
                        free(v);

                }
                break;
        }
        case BILIA_DATA_IN:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = 1;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_GE;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, matching_w(int_l->vname, bag_r->vname));
                bilia_dterm_array_push(geq->p.targs, un);
                bilia_dform_free(*replace);
                *replace = geq;
                break;
        }
        case BILIA_DATA_INN:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                long n = bilia_vector_at(f->p.targs, 1)->p.value;
                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = n;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_GE;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, matching_w(int_l->vname, bag_r->vname));
                bilia_dterm_array_push(geq->p.targs, un);
                bilia_dform_free(*replace);
                *replace = geq;
                break;
        }
        case BILIA_DATA_NI:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = 0;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_EQ;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, matching_w(int_l->vname, bag_r->vname));
                bilia_dterm_array_push(geq->p.targs, un);
                bilia_dform_free(*replace);
                *replace = geq;
                break;
        }
        default:
                break;
        }
}

void bilia_step3()
{
        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_rform()); i++)
                new_constraint(bilia_vector_at(bilia_entl_get_rform(), i),&bilia_vector_at(bilia_entl_get_rform(), i));

        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_nform()); i++)
                new_constraint(bilia_vector_at(bilia_entl_get_nform(), i),&bilia_vector_at(bilia_entl_get_nform(), i));

        if (bilia_option_get_verb () > 0)
        {
                fprintf(stdout, "\n=================================================\n");
                bilia_entl_fprint(stdout);
                fflush(stdout);
        }
}
