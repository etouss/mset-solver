


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

#include "bilia_step3_UF.h"

/*Create e dterm VAR_BAG(VAR_INT)*/
bilia_dterm_t * app_bag_int(bilia_dterm_t * var_int, bilia_dterm_t * var_bag)
{
        bilia_dterm_t * res = bilia_dterm_new();
        res->typ = BILIA_TYP_INT;
        res->kind = BILIA_DATA_APP;
        res->args = bilia_dterm_array_new();
        bilia_dterm_array_push(res->args, bilia_dterm_copy(var_bag));
        bilia_dterm_array_push(res->args, bilia_dterm_copy(var_int));
        return res;
}

bilia_dterm_t * construct_uf(bilia_dterm_t * var_int,bilia_dterm_array * array,int debut,bilia_data_op_t op){
        if(debut == BILIA_VECTOR_SIZE(array)-1)
                return app_bag_int(var_int, bilia_vector_at(array,debut));
        bilia_dterm_t * max = bilia_dterm_new();
        max->kind = op;
        max->typ = BILIA_TYP_INT;
        max->args = bilia_dterm_array_new();
        bilia_dterm_array_push(max->args, app_bag_int(var_int, bilia_vector_at(array,debut)));
        bilia_dterm_array_push(max->args, construct_uf(var_int,array,debut+1,op));
        return max;
}

/*Apply small model property rewritting to every litterals*/
void new_constraint_uf(bilia_dform_t * f,bilia_dform_t ** replace)
{
        bilia_var_array *  lvars  = bilia_entl_get_var_array();
        /* bilia_dform_t * f; */
        switch (f->kind)
        {
        case BILIA_DATA_AND:
        case BILIA_DATA_OR:
        {
                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(f->p.bargs); i++)
                        new_constraint_uf(bilia_vector_at(f->p.bargs,i),&bilia_vector_at(f->p.bargs,i));
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
                        if (tr->typ == BILIA_TYP_INT ) return;
                        else
                        {
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if (!var_int->element || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), tl), app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), tr)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                        }
                        break;
                }
                case BILIA_DATA_BAGUNION:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_MAX;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 0)));
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 1)));
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), construct_uf(var,tr->args,0,BILIA_DATA_MAX)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }
                case BILIA_DATA_BAGINTER:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_MIN;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 0)));
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 1)));
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), construct_uf(var,tr->args,0,BILIA_DATA_MIN)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }

                case BILIA_DATA_BAGSUM:
                {
                        if(!bilia_option_get_nr()) {
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dterm_t * max = bilia_dterm_new();
                                        max->kind = BILIA_DATA_PLUS;
                                        max->typ = BILIA_TYP_INT;
                                        max->args = bilia_dterm_array_new();
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 0)));
                                        bilia_dterm_array_push(max->args, app_bag_int(var, bilia_vector_at(tr->args, 1)));
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), max));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                        else{
                                bilia_dform_t * and = bilia_dform_new();
                                and->typ = BILIA_TYP_BOOL;
                                and->kind = BILIA_DATA_AND;
                                and->p.bargs = bilia_dform_array_new();
                                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                                {
                                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), construct_uf(var,tr->args,0,BILIA_DATA_PLUS)));
                                }
                                bilia_dform_free(*replace);
                                *replace = and;
                                break;
                        }
                }

                case BILIA_DATA_BAGMINUS:
                {
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dterm_t * minus = bilia_dterm_new();
                                minus->kind = BILIA_DATA_MINUS;
                                minus->typ = BILIA_TYP_INT;
                                minus->args = bilia_dterm_array_new();
                                bilia_dterm_array_push(minus->args, app_bag_int(var, bilia_vector_at(tr->args, 0)));
                                bilia_dterm_array_push(minus->args, app_bag_int(var, bilia_vector_at(tr->args, 1)));
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
                                bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(var, tl), max));
                        }
                        bilia_dform_free(*replace);
                        *replace = and;
                        break;
                }
                case BILIA_DATA_BAG:
                {
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        bilia_dterm_t * ta0 =  bilia_vector_at(tr->args, 0);
                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = 1;
                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(ta0, tl), un));

                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vid == ta0->p.sid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_NEQ;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(ta0));
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, tl), zero);
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
                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();
                        bilia_dterm_t * ta0 =  bilia_vector_at(tr->args, 0);
                        long n =  bilia_vector_at(tr->args, 1)->p.value;
                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = n;
                        bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(ta0, tl), un));

                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == ta0->p.sid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_NEQ;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(ta0));
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT));
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, tl), zero);
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
                        bilia_dform_t * or = bilia_dform_new();
                        or->kind = BILIA_DATA_OR;
                        or->typ = BILIA_TYP_BOOL;
                        or->p.bargs = bilia_dform_array_new();

                        bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                        bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                        bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);
                        bilia_var_t * var_bot = bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"bot"));
                        bilia_var_t * var_top = bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"top"));

                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = 1;

                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();

                        /*F(MAX) >= 1*/
                        bilia_dform_t * geq = bilia_dform_new();
                        geq->typ = BILIA_TYP_INT;
                        geq->kind = BILIA_DATA_GE;
                        geq->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(geq->p.targs, app_bag_int(tl, bag));
                        bilia_dterm_array_push(geq->p.targs, un);

                        /*diff bot and top*/
                        bilia_dform_t * bot = bilia_dform_new();
                        bot->typ = BILIA_TYP_BOOL;
                        bot->kind = BILIA_DATA_NEQ;
                        bot->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(bot->p.targs, bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT));
                        bilia_dterm_array_push(bot->p.targs, bilia_dterm_new_var(var_bot->vid,BILIA_TYP_INT));

                        bilia_dform_t * top = bilia_dform_new();
                        top->typ = BILIA_TYP_BOOL;
                        top->kind = BILIA_DATA_NEQ;
                        top->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(top->p.targs, bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT));
                        bilia_dterm_array_push(top->p.targs, bilia_dterm_new_var(var_top->vid,BILIA_TYP_INT));

                        bilia_dform_array_push(and->p.bargs, geq);
                        bilia_dform_array_push(and->p.bargs, bot);
                        bilia_dform_array_push(and->p.bargs, top);

                        /*max value*/
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if (!var_int->element   || var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
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
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, bag), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                        }
                        bilia_dform_array_push(or->p.bargs,and);

                        /*ELSE bot / top*/
                        bilia_dform_t * and2 = bilia_dform_new();
                        and2->typ = BILIA_TYP_BOOL;
                        and2->kind = BILIA_DATA_AND;
                        and2->p.bargs = bilia_dform_array_new();
                        /* = top*/
                        bilia_dform_array_push(and2->p.bargs,bilia_dform_new_eq(bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT),bilia_dterm_new_var(var_top->vid,BILIA_TYP_INT)));
                        /*= emptybag*/
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if (!var_int->element  || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dform_array_push(and2->p.bargs, bilia_dform_new_eq(app_bag_int(var, bag), zero));
                        }
                        bilia_dform_array_push(or->p.bargs,and2);
                        bilia_dform_free(*replace);
                        *replace = or;
                        break;


                        /*bilia_dform_t * and = bilia_dform_new();
                           and->typ = BILIA_TYP_BOOL;
                           and->kind = BILIA_DATA_AND;
                           and->p.bargs = bilia_dform_array_new();
                           bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                           bilia_dterm_t * un = bilia_dterm_new();
                           un->kind = BILIA_DATA_INT;
                           un->typ = BILIA_TYP_INT;
                           un->p.value = 1;

                           bilia_dform_t * geq = bilia_dform_new();
                           geq->typ = BILIA_TYP_INT;
                           geq->kind = BILIA_DATA_GE;
                           geq->p.targs = bilia_dterm_array_new();
                           bilia_dterm_array_push(geq->p.targs, app_bag_int(tl, bag));
                           bilia_dterm_array_push(geq->p.targs, un);
                           bilia_dform_array_push(and->p.bargs, geq);

                           for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                           {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == tl->p.sid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_LT;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, var);
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, bag), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                           }
                           bilia_dform_free(*replace);
                         * replace = and;
                           break;
                         */
                }
                case BILIA_DATA_BAGMAX:
                {
                        bilia_dform_t * or = bilia_dform_new();
                        or->kind = BILIA_DATA_OR;
                        or->typ = BILIA_TYP_BOOL;
                        or->p.bargs = bilia_dform_array_new();

                        bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                        bilia_var_t * var_int_l = bilia_vector_at(lvars, tl->p.sid);
                        bilia_var_t * var_bag_in = bilia_vector_at(lvars, bag->p.sid);
                        bilia_var_t * var_bot = bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"bot"));
                        bilia_var_t * var_top = bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"top"));

                        bilia_dterm_t * un = bilia_dterm_new();
                        un->kind = BILIA_DATA_INT;
                        un->typ = BILIA_TYP_INT;
                        un->p.value = 1;

                        bilia_dform_t * and = bilia_dform_new();
                        and->typ = BILIA_TYP_BOOL;
                        and->kind = BILIA_DATA_AND;
                        and->p.bargs = bilia_dform_array_new();

                        /*F(MAX) >= 1*/
                        bilia_dform_t * geq = bilia_dform_new();
                        geq->typ = BILIA_TYP_INT;
                        geq->kind = BILIA_DATA_GE;
                        geq->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(geq->p.targs, app_bag_int(tl, bag));
                        bilia_dterm_array_push(geq->p.targs, un);

                        /*diff bot and top*/
                        bilia_dform_t * bot = bilia_dform_new();
                        bot->typ = BILIA_TYP_BOOL;
                        bot->kind = BILIA_DATA_NEQ;
                        bot->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(bot->p.targs, bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT));
                        bilia_dterm_array_push(bot->p.targs, bilia_dterm_new_var(var_bot->vid,BILIA_TYP_INT));

                        bilia_dform_t * top = bilia_dform_new();
                        top->typ = BILIA_TYP_BOOL;
                        top->kind = BILIA_DATA_NEQ;
                        top->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(top->p.targs, bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT));
                        bilia_dterm_array_push(top->p.targs, bilia_dterm_new_var(var_top->vid,BILIA_TYP_INT));

                        bilia_dform_array_push(and->p.bargs, geq);
                        bilia_dform_array_push(and->p.bargs, bot);
                        bilia_dform_array_push(and->p.bargs, top);

                        /*max value*/
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if (!var_int->element   || var_int->vid == var_int_l->vid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
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
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, bag), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                        }
                        bilia_dform_array_push(or->p.bargs,and);

                        /*ELSE bot / top*/
                        bilia_dform_t * and2 = bilia_dform_new();
                        and2->typ = BILIA_TYP_BOOL;
                        and2->kind = BILIA_DATA_AND;
                        and2->p.bargs = bilia_dform_array_new();
                        /* = top*/
                        bilia_dform_array_push(and2->p.bargs,bilia_dform_new_eq(bilia_dterm_new_var(tl->p.sid,BILIA_TYP_INT),bilia_dterm_new_var(var_bot->vid,BILIA_TYP_INT)));
                        /*= emptybag*/
                        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                        {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if (!var_int->element  || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dform_array_push(and2->p.bargs, bilia_dform_new_eq(app_bag_int(var, bag), zero));
                        }
                        bilia_dform_array_push(or->p.bargs,and2);
                        bilia_dform_free(*replace);
                        *replace = or;
                        break;

                        /*bilia_dform_t * and = bilia_dform_new();
                           and->typ = BILIA_TYP_BOOL;
                           and->kind = BILIA_DATA_AND;
                           and->p.bargs = bilia_dform_array_new();
                           bilia_dterm_t * bag =  bilia_vector_at(tr->args, 0);
                           bilia_dterm_t * un = bilia_dterm_new();
                           un->kind = BILIA_DATA_INT;
                           un->typ = BILIA_TYP_INT;
                           un->p.value = 1;

                           bilia_dform_t * geq = bilia_dform_new();
                           geq->typ = BILIA_TYP_INT;
                           geq->kind = BILIA_DATA_GE;
                           geq->p.targs = bilia_dterm_array_new();
                           bilia_dterm_array_push(geq->p.targs, app_bag_int(tl, bag));
                           bilia_dterm_array_push(geq->p.targs, un);
                           bilia_dform_array_push(and->p.bargs, geq);

                           for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                           {
                                bilia_var_t * var_int = bilia_vector_at(lvars, i);
                                if ((!var_int->element && bilia_option_get_optim_ele()) || var_int->vid == tl->p.sid || var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                                bilia_dterm_t * zero = bilia_dterm_new();
                                zero->kind = BILIA_DATA_INT;
                                zero->typ = BILIA_TYP_INT;
                                zero->p.value = 0;
                                bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                                bilia_dform_t * impl = bilia_dform_new();
                                impl->kind = BILIA_DATA_GT;
                                impl->typ = BILIA_TYP_INT;
                                impl->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(impl->p.targs, var);
                                bilia_dterm_array_push(impl->p.targs, bilia_dterm_copy(tl));
                                bilia_dform_t * impr = bilia_dform_new_eq(app_bag_int(var, bag), zero);
                                bilia_dform_t * imp = bilia_dform_new();
                                imp->typ = BILIA_TYP_INT;
                                imp->kind = BILIA_DATA_IMPLIES;
                                imp->p.bargs = bilia_dform_array_new();
                                bilia_dform_array_push(imp->p.bargs, impl);
                                bilia_dform_array_push(imp->p.bargs, impr);
                                bilia_dform_array_push(and->p.bargs, imp);
                           }
                           bilia_dform_free(*replace);
                         * replace = and;
                           break;
                         */
                }
                case BILIA_DATA_EMPTYBAG:
                {
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
                                bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), tl), zero));
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
                for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                {
                        bilia_var_t * var_int = bilia_vector_at(lvars, i);
                        if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                        bilia_dterm_t * var = bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT);
                        bilia_dform_t * leq = bilia_dform_new();
                        leq->kind = BILIA_DATA_LE;
                        leq->typ = BILIA_TYP_INT;
                        leq->p.targs = bilia_dterm_array_new();
                        bilia_dterm_array_push(leq->p.targs, app_bag_int(var, bilia_vector_at(f->p.targs, 0)));
                        bilia_dterm_array_push(leq->p.targs, app_bag_int(var, bilia_vector_at(f->p.targs, 1)));
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
                bilia_var_t * bag_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                bilia_dform_t * distinct = bilia_dform_new();
                distinct->kind = BILIA_DATA_NEQ;
                distinct->typ = BILIA_TYP_INT;
                distinct->p.targs = bilia_dterm_array_new();
                uint_t sid = UNDEFINED_ID;
                char * v = malloc(strlen(bag_l->vname) + strlen(bag_r->vname) + 4);
                sprintf(v, "%sneq%s", bag_l->vname, bag_r->vname);
                if ((sid = bilia_var_array_find_local(lvars, v)) == UNDEFINED_ID)
                {
                        sprintf(v, "%sneq%s", bag_r->vname, bag_l->vname);
                        sid = bilia_var_array_find_local(lvars, v);
                }
                bilia_dterm_array_push(distinct->p.targs, app_bag_int(bilia_dterm_new_var(sid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 0)));
                bilia_dterm_array_push(distinct->p.targs, app_bag_int(bilia_dterm_new_var(sid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 1)));
                bilia_dform_free(*replace);
                *replace = distinct;
                free(v);
                break;
        }
        case BILIA_DATA_NSUBSETEQ:
        {
                bilia_var_t * bag_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);
                bilia_var_t * bag_r = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 1)->p.sid);
                bilia_dform_t * distinct = bilia_dform_new();
                distinct->kind = BILIA_DATA_GT;
                distinct->typ = BILIA_TYP_INT;
                distinct->p.targs = bilia_dterm_array_new();
                uint_t sid = UNDEFINED_ID;
                char * v = malloc(strlen(bag_l->vname) + strlen(bag_r->vname) + 10);
                sprintf(v, "%snsubseteq%s", bag_l->vname, bag_r->vname);
                sid = bilia_var_array_find_local(lvars, v);
                bilia_dterm_array_push(distinct->p.targs, app_bag_int(bilia_dterm_new_var(sid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 0)));
                bilia_dterm_array_push(distinct->p.targs, app_bag_int(bilia_dterm_new_var(sid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 1)));
                bilia_dform_free(*replace);
                *replace = distinct;
                free(v);
                break;
        }
        case BILIA_DATA_IN:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);

                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = 1;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_GE;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, app_bag_int(bilia_vector_at(f->p.targs, 0), bilia_vector_at(f->p.targs, 1)));
                bilia_dterm_array_push(geq->p.targs, un);
                if(!int_l->extremum){
                  bilia_dform_free(*replace);
                  *replace = geq;
                  break;
                }
                else{
                  bilia_dform_t * or_top = bilia_dform_new();
                  or_top->kind = BILIA_DATA_OR;
                  or_top->typ = BILIA_TYP_INT;
                  or_top->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * or_bot = bilia_dform_new();
                  or_bot->kind = BILIA_DATA_OR;
                  or_bot->typ = BILIA_TYP_INT;
                  or_bot->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * and_bot = bilia_dform_new();
                  and_bot->kind = BILIA_DATA_AND;
                  and_bot->typ = BILIA_TYP_INT;
                  and_bot->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * neq_bot = bilia_dform_new();
                  neq_bot->kind = BILIA_DATA_EQ;
                  neq_bot->typ = BILIA_TYP_INT;
                  neq_bot->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"bot"))->vid,BILIA_TYP_INT));

                  bilia_dform_t * neq_top = bilia_dform_new();
                  neq_top->kind = BILIA_DATA_EQ;
                  neq_top->typ = BILIA_TYP_INT;
                  neq_top->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"top"))->vid,BILIA_TYP_INT));

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
                          bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 1)), zero));
                  }
                  bilia_dform_array_push(or_bot->p.bargs,neq_bot);
                  bilia_dform_array_push(or_bot->p.bargs,neq_top);
                  bilia_dform_array_push(and_bot->p.bargs,or_bot);
                  bilia_dform_array_push(and_bot->p.bargs,and);
                  bilia_dform_array_push(or_top->p.bargs,and_bot);
                  bilia_dform_array_push(or_top->p.bargs,geq);

                  bilia_dform_free(*replace);
                  *replace = or_top;
                  break;
              }
        }
        case BILIA_DATA_INN:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);

                long n = bilia_vector_at(f->p.targs, 1)->p.value;
                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = n;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_GE;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, app_bag_int(bilia_vector_at(f->p.targs, 0), bilia_vector_at(f->p.targs, 1)));
                bilia_dterm_array_push(geq->p.targs, un);
                if(!int_l->extremum){
                  bilia_dform_free(*replace);
                  *replace = geq;
                  break;
                }
                else{
                  bilia_dform_t * and_bot = bilia_dform_new();
                  and_bot->kind = BILIA_DATA_AND;
                  and_bot->typ = BILIA_TYP_INT;
                  and_bot->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * neq_bot = bilia_dform_new();
                  neq_bot->kind = BILIA_DATA_NEQ;
                  neq_bot->typ = BILIA_TYP_INT;
                  neq_bot->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"bot"))->vid,BILIA_TYP_INT));

                  bilia_dform_t * neq_top = bilia_dform_new();
                  neq_top->kind = BILIA_DATA_NEQ;
                  neq_top->typ = BILIA_TYP_INT;
                  neq_top->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"top"))->vid,BILIA_TYP_INT));

                  bilia_dform_array_push(and_bot->p.bargs,neq_bot);
                  bilia_dform_array_push(and_bot->p.bargs,neq_top);
                  bilia_dform_array_push(and_bot->p.bargs,geq);

                  bilia_dform_free(*replace);
                  *replace = and_bot;
                  break;
                }
        }
        // case BILIA_DATA_INNE:
        // {
        //   long n = bilia_vector_at(f->p.targs, 1)->p.value;
        //   bilia_dterm_t * un = bilia_dterm_new();
        //   un->kind = BILIA_DATA_INT;
        //   un->typ = BILIA_TYP_INT;
        //   un->p.value = n;
        //   bilia_dform_t * geq = bilia_dform_new();
        //   geq->kind = BILIA_DATA_EQ;
        //   geq->typ = BILIA_TYP_INT;
        //   geq->p.targs = bilia_dterm_array_new();
        //   bilia_dterm_array_push(geq->p.targs, app_bag_int(bilia_vector_at(f->p.targs, 0), bilia_vector_at(f->p.targs, 1)));
        //   bilia_dterm_array_push(geq->p.targs, un);
        //   if (target)bilia_pure_add_dform(target, geq);
        //   break;
        // }
        case BILIA_DATA_NI:
        {
                bilia_var_t * int_l = bilia_vector_at(lvars, bilia_vector_at(f->p.targs, 0)->p.sid);

                bilia_dterm_t * un = bilia_dterm_new();
                un->kind = BILIA_DATA_INT;
                un->typ = BILIA_TYP_INT;
                un->p.value = 0;
                bilia_dform_t * geq = bilia_dform_new();
                geq->kind = BILIA_DATA_EQ;
                geq->typ = BILIA_TYP_INT;
                geq->p.targs = bilia_dterm_array_new();
                bilia_dterm_array_push(geq->p.targs, app_bag_int(bilia_vector_at(f->p.targs, 0), bilia_vector_at(f->p.targs, 1)));
                bilia_dterm_array_push(geq->p.targs, un);
                if(!int_l->extremum){
                  bilia_dform_free(*replace);
                  *replace = geq;
                  break;
                }
                else{
                  bilia_dform_t * or_top = bilia_dform_new();
                  or_top->kind = BILIA_DATA_AND;
                  or_top->typ = BILIA_TYP_INT;
                  or_top->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * or_bot = bilia_dform_new();
                  or_bot->kind = BILIA_DATA_AND;
                  or_bot->typ = BILIA_TYP_INT;
                  or_bot->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * and_bot = bilia_dform_new();
                  and_bot->kind = BILIA_DATA_OR;
                  and_bot->typ = BILIA_TYP_INT;
                  and_bot->p.bargs = bilia_dform_array_new();

                  bilia_dform_t * neq_bot = bilia_dform_new();
                  neq_bot->kind = BILIA_DATA_NEQ;
                  neq_bot->typ = BILIA_TYP_INT;
                  neq_bot->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_bot->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"bot"))->vid,BILIA_TYP_INT));

                  bilia_dform_t * neq_top = bilia_dform_new();
                  neq_top->kind = BILIA_DATA_NEQ;
                  neq_top->typ = BILIA_TYP_INT;
                  neq_top->p.targs = bilia_dterm_array_new();
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_copy(bilia_vector_at(f->p.targs, 0)));
                  bilia_dterm_array_push(neq_top->p.targs, bilia_dterm_new_var(bilia_vector_at(lvars,bilia_var_array_find_local(lvars,"top"))->vid,BILIA_TYP_INT));

                  bilia_dform_t * and = bilia_dform_new();
                  and->typ = BILIA_TYP_BOOL;
                  and->kind = BILIA_DATA_OR;
                  and->p.bargs = bilia_dform_array_new();
                  for (uint_t i = 0; i < BILIA_VECTOR_SIZE(lvars); i++)
                  {
                          bilia_var_t * var_int = bilia_vector_at(lvars, i);
                          if ((!var_int->element && bilia_option_get_optim_ele())|| var_int->vty->kind != BILIA_TYP_INT || !var_int->used) continue;
                          bilia_dterm_t * zero = bilia_dterm_new();
                          zero->kind = BILIA_DATA_INT;
                          zero->typ = BILIA_TYP_INT;
                          zero->p.value = 0;
                          bilia_dform_t * neq = bilia_dform_new();
                          neq->kind = BILIA_DATA_NEQ;
                          neq->typ = BILIA_TYP_INT;
                          neq->p.targs = bilia_dterm_array_new();
                          bilia_dterm_array_push(neq->p.targs,app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 1)));
                          bilia_dterm_array_push(neq->p.targs,zero);
                          bilia_dform_array_push(and->p.bargs, neq);
                          //bilia_dform_array_push(and->p.bargs, bilia_dform_new_eq(app_bag_int(bilia_dterm_new_var(var_int->vid, BILIA_TYP_INT), bilia_vector_at(f->p.targs, 1)), zero));
                  }
                  bilia_dform_array_push(or_bot->p.bargs,neq_bot);
                  bilia_dform_array_push(or_bot->p.bargs,neq_top);
                  bilia_dform_array_push(and_bot->p.bargs,or_bot);
                  bilia_dform_array_push(and_bot->p.bargs,and);
                  bilia_dform_array_push(or_top->p.bargs,and_bot);
                  bilia_dform_array_push(or_top->p.bargs,geq);

                  bilia_dform_free(*replace);
                  *replace = or_top;
                  break;
              }
        }
        default:
                break;
        }
        return;
}

void bilia_step3_uf()
{
        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_rform()); i++)
                new_constraint_uf(bilia_vector_at(bilia_entl_get_rform(), i),&bilia_vector_at(bilia_entl_get_rform(), i));

        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_nform()); i++)
                new_constraint_uf(bilia_vector_at(bilia_entl_get_nform(), i),&bilia_vector_at(bilia_entl_get_nform(), i));

        if (bilia_option_get_verb () > 0)
        {
                fprintf(stdout, "\n=================================================\n");
                bilia_entl_fprint(stdout);
                fflush(stdout);
        }
}
