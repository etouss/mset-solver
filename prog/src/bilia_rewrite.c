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

#include "bilia_rewrite.h"
extern bilia_type_t * tybool;
extern bilia_type_t * tybag;
extern bilia_type_t * tyint;
#ifdef BENCH
int nb_constraint_before = 0;
#endif


void quick_sorted_char_list(char ** list,int nb_term){
        int i, j;
        char * p;
        char * t;
        if (nb_term < 2)
                return;
        p = list[nb_term / 2];
        for (i = 0, j = nb_term - 1;; i++, j--) {
                while (strcmp(list[i],p))
                        i++;
                while (strcmp(p,list[j]))
                        j--;
                if (i >= j)
                        break;
                t = list[i];
                list[i] = list[j];
                list[j] = t;
        }
        quick_sorted_char_list(list, i);
        quick_sorted_char_list(list + i, nb_term - i);
}

char * create_var_from_dterm(bilia_dterm_array * array,bilia_data_op_t type){
        const char * stype;
        switch(type) {
        case BILIA_DATA_BAGUNION:
                stype = "UNION";
                break;
        case BILIA_DATA_BAGSUM:
                stype = "SUM";
                break;
        case BILIA_DATA_BAGINTER:
                stype = "INTER";
                break;
        case BILIA_DATA_BAGMINUS:
                stype = "MINUS";
                break;
        }
        bilia_var_array * lvars = bilia_entl_get_var_array();
        int nb_term = BILIA_VECTOR_SIZE(array);
        char * list[nb_term];
        int taille = 0;
        for(int i = 0; i<nb_term; i++) {
                if(bilia_vector_at(array,i)->kind == BILIA_DATA_VAR)
                        list[i] = bilia_vector_at(lvars,bilia_vector_at(array,i)->p.sid)->vname;
                else
                        list[i] = create_var_from_dterm(bilia_vector_at(array,i)->args,bilia_vector_at(array,i)->kind);
                taille += strlen(list[i]);
        }
        char * result = malloc((taille + 1 + nb_term*strlen(stype))*sizeof(char));
        memset(result,0,(taille + 1 + nb_term*strlen(stype)));
        if(type != BILIA_DATA_BAGMINUS)
                quick_sorted_char_list(list,nb_term);
        for(int i = 0; i<nb_term; i++) {
                strcat(result,list[i]);
                strcat(result,stype);
        }
        /*MEMORY LEAK*/
        return result;
}


/*Creation des bilia_dterm_t issu des term (Union Inter etc)*/
bilia_dterm_t * bilia_apply_create(bilia_dterm_t * ndterm, bilia_dform_array * rarray,bool top)
{
        /* Creation du bilia_dterm_t */
        if (ndterm->args == NULL)
                return ndterm;
        // bilia_dterm_t * ndterm = bilia_dterm_new();
        // ndterm->kind = dterm->kind;
        // ndterm->typ = dterm->typ;
        // ndterm->p = dterm->p;
        // ndterm->args = bilia_dterm_array_new();
        /* bilia_dterm_array_reserve(ndterm->args, BILIA_VECTOR_SIZE(dterm->args)); */
        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(ndterm->args); i++)
                bilia_vector_at(ndterm->args, i) = bilia_apply_create(bilia_vector_at(ndterm->args, i),rarray,false);
        //bilia_dterm_array_push (ndterm->args, bilia_apply_create(bilia_vector_at(dterm->args, i),rarray),rarray));
        /*Appelle recursif pour creuser la fonction avant d'appliquer*/
        /*CREATION d'une variable en trop   B = BT*/
        /* bilia_dterm_array *args */
        //if (top && dterm->kind != BILIA_DATA_ITE)return ndterm;

        //bilia_dterm_free(dterm);
        //bilia_dterm_array_delete(dterm->args);
        //free(dterm);

        char * v0 = NULL;
        char * v1 = NULL;
        char * v = NULL;
        bilia_dterm_t * retour = NULL;
        bool ajout = false;
        switch (ndterm->kind)
        {
        case BILIA_DATA_BAGUNION:
                if(top) return ndterm;
                if(!bilia_option_get_nr()) {
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(ndterm->args, 1)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_BAGINT);
                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 1)->p.sid, BILIA_TYP_BAGINT);
                        v = malloc(strlen(v0) + strlen(v1) + strlen("BAGUNION") + 1);
                        sprintf(v, "%sBAGUNION%s", v0, v1);
                        /*After recursif App every T are suppose to be variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                sprintf(v, "%sBAGUNION%s", v1, v0);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                {
                                        /*CREATION*/
                                        /*if xuniony and yunionx dont exists we create the variable*/
                                        bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                                        ajout = true;
                                }
                        }
                        /*We send back the variable*/
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                /*if the variable is new we constrain her*/
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else{
//                         char * list[BILIA_VECTOR_SIZE(ndterm->args)];
//                         for (uint_t i = 0; i < BILIA_VECTOR_SIZE(ndterm->args); i++)
//                                 list[i] = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, i)->p.sid, BILIA_TYP_BAGINT);
//                         char * var_name = create_var_from_list(list,BILIA_VECTOR_SIZE(ndterm->args),"UNION");
//                         if (bilia_var_array_find_local(bilia_entl_get_var_array(), var_name) == UNDEFINED_ID)
//                         {
//                                 /*CREATION*/
//                                 /*if xuniony and yunionx dont exists we create the variable*/
//                                 bilia_var_register (bilia_entl_get_var_array(), var_name, tybag, BILIA_SCOPE_GLOBAL,false);
//                                 ajout = true;
//                         }
//                         /*We send back the variable*/
//                         retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), var_name), BILIA_TYP_BAGINT);
//                         if (ajout)
//                         {
// #ifdef BENCH
//                                 nb_constraint_before++;
// #endif
//                                 /*if the variable is new we constrain her*/
//                                 bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
//                                 bilia_dform_array_push(rarray, tmp);
//                         }
//                         free(var_name);
//                         return retour;
                        return ndterm;
                }
        case BILIA_DATA_BAGINTER:
                if(top) return ndterm;
                if(!bilia_option_get_nr()) {
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(ndterm->args, 1)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_BAGINT);
                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 1)->p.sid, BILIA_TYP_BAGINT);
                        v = malloc(strlen(v0) + strlen(v1) + strlen("BAGINTER") + 1);
                        sprintf(v, "%sBAGINTER%s", v0, v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                sprintf(v, "%sBAGINTER%s", v1, v0);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                {
                                        /*CREATION*/
                                        /*On ajoute a rform v = v0 BAGMINUS v1*/
                                        bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                                        ajout = true;
                                }
                        }
                        /*On renvoie juste la variable*/
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else{
//                         char * list[BILIA_VECTOR_SIZE(ndterm->args)];
//                         for (uint_t i = 0; i < BILIA_VECTOR_SIZE(ndterm->args); i++)
//                                 list[i] = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, i)->p.sid, BILIA_TYP_BAGINT);
//                         char * var_name = create_var_from_list(list,BILIA_VECTOR_SIZE(ndterm->args),"BAGINTER");
//                         if (bilia_var_array_find_local(bilia_entl_get_var_array(), var_name) == UNDEFINED_ID)
//                         {
//                                 /*CREATION*/
//                                 /*if xuniony and yunionx dont exists we create the variable*/
//                                 bilia_var_register (bilia_entl_get_var_array(), var_name, tybag, BILIA_SCOPE_GLOBAL,false);
//                                 ajout = true;
//                         }
//                         /*We send back the variable*/
//                         retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), var_name), BILIA_TYP_BAGINT);
//                         if (ajout)
//                         {
// #ifdef BENCH
//                                 nb_constraint_before++;
// #endif
//                                 /*if the variable is new we constrain her*/
//                                 bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
//                                 bilia_dform_array_push(rarray, tmp);
//                         }
//                         free(var_name);
//                         return retour;
                        return ndterm;
                }
        /*Creation d'une memory leak*/
        case BILIA_DATA_BAGMINUS:
                if(top) return ndterm;
                if(!bilia_option_get_nr()) {
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(ndterm->args, 1)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_BAGINT);
                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 1)->p.sid, BILIA_TYP_BAGINT);
                        v = malloc(strlen(v0) + strlen(v1) + strlen("BAGMINUS") + 1);
                        sprintf(v, "%sBAGMINUS%s", v0, v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                                ajout = true;
                        }
                        /*On renvoie juste la variable*/
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else
                        return ndterm;
        case BILIA_DATA_BAGSUM:
                if(top) return ndterm;
                if(!bilia_option_get_nr()) {
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(ndterm->args, 1)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_BAGINT);
                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 1)->p.sid, BILIA_TYP_BAGINT);
                        v = malloc(strlen(v0) + strlen(v1) + strlen("BAGSUM") + 1);
                        sprintf(v, "%sBAGSUM%s", v0, v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                sprintf(v, "%sBAGSUM%s", v1, v0);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                {
                                        /*CREATION*/
                                        /*On ajoute a rform v = v0 BAGMINUS v1*/
                                        bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                                        ajout = true;
                                }
                        }
                        /*On renvoie juste la variable*/
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else{
//                         char * list[BILIA_VECTOR_SIZE(ndterm->args)];
//                         for (uint_t i = 0; i < BILIA_VECTOR_SIZE(ndterm->args); i++)
//                                 list[i] = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, i)->p.sid, BILIA_TYP_BAGINT);
//                         char * var_name = create_var_from_list(list,BILIA_VECTOR_SIZE(ndterm->args),"BAGSUM");
//                         if (bilia_var_array_find_local(bilia_entl_get_var_array(), var_name) == UNDEFINED_ID)
//                         {
//                                 /*CREATION*/
//                                 /*if xuniony and yunionx dont exists we create the variable*/
//                                 bilia_var_register (bilia_entl_get_var_array(), var_name, tybag, BILIA_SCOPE_GLOBAL,false);
//                                 ajout = true;
//                         }
//                         /*We send back the variable*/
//                         retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), var_name), BILIA_TYP_BAGINT);
//                         if (ajout)
//                         {
// #ifdef BENCH
//                                 nb_constraint_before++;
// #endif
//                                 /*if the variable is new we constrain her*/
//                                 bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
//                                 bilia_dform_array_push(rarray, tmp);
//                         }
//                         free(var_name);
//                         return retour;
                        return ndterm;
                }
        /*Creation d'une memory leak*/
        case BILIA_DATA_BAG:
                if(top) return ndterm;
                assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                v = malloc(strlen(v0) + strlen("BAG") + 1);
                sprintf(v, "BAG%s", v0);
                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                {
                        bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                        ajout = true;
                }
                retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                if (ajout)
                {
      #ifdef BENCH
                        nb_constraint_before++;
      #endif
                        bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                        bilia_dform_array_push(rarray, tmp);
                }
                free(v);
                return retour;

                break;
        case BILIA_DATA_BAGN:
                if(top) return ndterm;
                assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                assert(bilia_vector_at(ndterm->args, 1)->kind == BILIA_DATA_INT);
                char str[15];
                sprintf(str, "%ld", bilia_vector_at(ndterm->args, 1)->p.value);
                v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                v = malloc(strlen(v0) + strlen("BAG") + strlen(str) + 1);
                sprintf(v, "BAG%s%s", str, v0);
                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                {
                        bilia_var_register (bilia_entl_get_var_array(), v, tybag, BILIA_SCOPE_GLOBAL,false);
                        ajout = true;
                }
                retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_BAGINT);
                if (ajout)
                {
      #ifdef BENCH
                        nb_constraint_before++;
      #endif
                        bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                        bilia_dform_array_push(rarray, tmp);
                }
                free(v);
                return retour;

                break;
        case BILIA_DATA_BAGMAX:
                if(top) return ndterm;
                if(!bilia_option_get_nr()) {
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                        v = malloc(strlen(v0) + strlen("bagmax") + 1);
                        sprintf(v, "bagmax%s", v0);
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajout = true;
                        }
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_INT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else
                {
                        char * v0;
                        if(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR)
                                v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                        else
                                v0 = create_var_from_dterm(bilia_vector_at(ndterm->args, 0)->args,bilia_vector_at(ndterm->args, 0)->kind);
                        v = malloc(strlen(v0) + strlen("bagmax") + 1);
                        sprintf(v, "bagmax%s", v0);
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajout = true;
                        }
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_INT);
                        if (ajout)
                        {
              #ifdef BENCH
                                nb_constraint_before++;
              #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
        case BILIA_DATA_BAGMIN:
                if(!bilia_option_get_nr()) {
                        if(top) return ndterm;
                        assert(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR);
                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                        v = malloc(strlen(v0) + strlen("bagmin") + 1);
                        sprintf(v, "bagmin%s", v0);
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajout = true;
                        }
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_INT);
                        if (ajout)
                        {
      #ifdef BENCH
                                nb_constraint_before++;
      #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
                else{
                        char * v0;
                        if(bilia_vector_at(ndterm->args, 0)->kind == BILIA_DATA_VAR)
                                v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(ndterm->args, 0)->p.sid, BILIA_TYP_INT);
                        else
                                v0 = create_var_from_dterm(bilia_vector_at(ndterm->args, 0)->args,bilia_vector_at(ndterm->args, 0)->kind);
                        v = malloc(strlen(v0) + strlen("bagmin") + 1);
                        sprintf(v, "bagmin%s", v0);
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajout = true;
                        }
                        retour = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), v), BILIA_TYP_INT);
                        if (ajout)
                        {
                  #ifdef BENCH
                                nb_constraint_before++;
                  #endif
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_copy(retour),ndterm);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(v);
                        return retour;
                }
        default:
                return ndterm;
                /*return le bilia_dterm_t tel quelle*/
                break;
        }
}

void bilia_create_nvar(bilia_dform_t * dform, bilia_dform_array * rarray)
{
        /*Variable creation for literal min max neq nsubset*/
        if(dform->kind == BILIA_DATA_EQ || dform->kind == BILIA_DATA_NEQ || dform->kind == BILIA_DATA_LE || dform->kind == BILIA_DATA_GE || dform->kind == BILIA_DATA_GT \
           || dform->kind == BILIA_DATA_LT || dform->kind == BILIA_DATA_INN  || dform->kind == BILIA_DATA_NIN  || dform->kind == BILIA_DATA_IN  || dform->kind == BILIA_DATA_NI \
           || dform->kind == BILIA_DATA_SUBSETEQ || dform->kind == BILIA_DATA_NSUBSETEQ) {
                if (dform->kind == BILIA_DATA_EQ)
                {
                        /*If Equalities we don't have to create new variable*/
                        if (bilia_vector_at (dform->p.targs, 0)->kind == BILIA_DATA_VAR)
                                bilia_vector_at (dform->p.targs, 1) = bilia_apply_create(bilia_vector_at (dform->p.targs, 1),rarray,true);
                        else if (bilia_vector_at (dform->p.targs, 1)->kind == BILIA_DATA_VAR)
                        {
                                /*Weird*/
                                bilia_dterm_t * un = bilia_vector_at (dform->p.targs, 1);
                                bilia_dterm_t * zero = bilia_vector_at (dform->p.targs, 0);
                                dform->p.targs = bilia_dterm_array_new();
                                bilia_dterm_array_push(dform->p.targs, un);
                                bilia_dterm_array_push(dform->p.targs, zero);
                                bilia_vector_at (dform->p.targs, 1) = bilia_apply_create(bilia_vector_at (dform->p.targs, 1),rarray,true);
                        }
                        else
                        {
                                for (uint_t j = 0; j < bilia_vector_size (dform->p.targs); j++)
                                        bilia_vector_at (dform->p.targs, j) = bilia_apply_create(bilia_vector_at (dform->p.targs, j),rarray,false);
                        }
                }
                else
                {
                        for (uint_t j = 0; j < bilia_vector_size (dform->p.targs); j++)
                                bilia_vector_at (dform->p.targs, j) = bilia_apply_create(bilia_vector_at (dform->p.targs, j),rarray,false);
                }
        }
        else{
                for (size_t i = 0; i < BILIA_VECTOR_SIZE(dform->p.bargs); i++)
                        bilia_create_nvar(bilia_vector_at(dform->p.bargs,i),rarray);
        }
        switch (dform->kind)
        {
        case BILIA_DATA_NEQ:
                /*We have to create a new variable xneqy or yneqx*/
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT)  break;
                {

                        if(!bilia_option_get_nr()) {
                                assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                                assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                                char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                                char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                                char * v = malloc(strlen(v0) + strlen(v1) + strlen("neq") + 1);
                                sprintf(v, "%sneq%s", v0, v1);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                {
                                        sprintf(v, "%sneq%s", v1, v0);
                                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID){
                                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                                bilia_vector_at(bilia_entl_get_var_array(),BILIA_VECTOR_SIZE(bilia_entl_get_var_array())-1)->used = true;
                                              }
                                }
                                free(v);
                        }
                        else{
                                char * v0;
                                if(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR)
                                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                                else
                                        v0 = create_var_from_dterm(bilia_vector_at(dform->p.targs, 0)->args,bilia_vector_at(dform->p.targs, 0)->kind);
                                char * v1;
                                if(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR)
                                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                                else
                                        v1 = create_var_from_dterm(bilia_vector_at(dform->p.targs, 1)->args,bilia_vector_at(dform->p.targs, 1)->kind);
                                char * v = malloc(strlen(v0) + strlen(v1) + strlen("neq") + 1);
                                sprintf(v, "%sneq%s", v0, v1);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                {
                                        sprintf(v, "%sneq%s", v1, v0);
                                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID){
                                                bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                                bilia_vector_at(bilia_entl_get_var_array(),BILIA_VECTOR_SIZE(bilia_entl_get_var_array())-1)->used = true;
                                              }
                                }
                                free(v);
                        }
                }
                break;
        case BILIA_DATA_NSUBSETEQ:
                /*we have to create a new variable xnsubsety  carefull xnsubsety diff ynsubsetx*/
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT)  break;
                {
                        if(!bilia_option_get_nr()) {
                                assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                                assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                                char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                                char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                                char * v = malloc(strlen(v0) + strlen(v1) + strlen("nsubseteq") + 1);
                                sprintf(v, "%snsubseteq%s", v0, v1);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                        bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                bilia_vector_at(bilia_entl_get_var_array(),BILIA_VECTOR_SIZE(bilia_entl_get_var_array())-1)->used = true;
                                free(v);
                        }
                        else{
                                char * v0;
                                if(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR)
                                        v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                                else
                                        v0 = create_var_from_dterm(bilia_vector_at(dform->p.targs, 0)->args,bilia_vector_at(dform->p.targs, 0)->kind);
                                char * v1;
                                if(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR)
                                        v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                                else
                                        v1 = create_var_from_dterm(bilia_vector_at(dform->p.targs, 1)->args,bilia_vector_at(dform->p.targs, 1)->kind);
                                char * v = malloc(strlen(v0) + strlen(v1) + strlen("snsubseteq") + 1);
                                sprintf(v, "%snsubseteq%s", v0, v1);
                                if (bilia_var_array_find_local(bilia_entl_get_var_array(), v) == UNDEFINED_ID)
                                      bilia_var_register (bilia_entl_get_var_array(), v, tyint, BILIA_SCOPE_GLOBAL,true);
                                bilia_vector_at(bilia_entl_get_var_array(),BILIA_VECTOR_SIZE(bilia_entl_get_var_array())-1)->used = true;
                                free(v);

                        }
                }
                break;
        case BILIA_DATA_GE:
                /*create a min / max variable*/
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT) break;
                {
                        assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                        char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                        char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                        bool ajoutd = false;
                        bool ajoutg = false;
                        char * vg = malloc(strlen(v0) + strlen("bagmin") + 1);
                        char * vd = malloc(strlen(v1) + strlen("bagmax") + 1);
                        sprintf(vg, "bagmin%s", v0);
                        sprintf(vd, "bagmax%s", v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vg) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vg, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutg = true;
                        }
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vd) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vd, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutd = true;
                        }
                        /*On renvoie juste la variable*/
                        bilia_dterm_t * dtg = bilia_vector_at(dform->p.targs, 0);
                        bilia_dterm_t * dtd = bilia_vector_at(dform->p.targs, 1);
                        bilia_vector_at(dform->p.targs, 0) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT);
                        bilia_vector_at(dform->p.targs, 1) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT);
                        dform->kind = BILIA_DATA_GE;
                        dform->typ = BILIA_TYP_INT;
                        if (ajoutd)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMAX;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtd);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        if (ajoutg)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMIN;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtg);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(vg);
                        free(vd);
                }
                break;
        case BILIA_DATA_GT:
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT) break;
                {
                        assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                        char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                        char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                        bool ajoutd = false;
                        bool ajoutg = false;
                        char * vg = malloc(strlen(v0) + strlen("bagmin") + 1);
                        char * vd = malloc(strlen(v1) + strlen("bagmax") + 1);
                        sprintf(vg, "bagmin%s", v0);
                        sprintf(vd, "bagmax%s", v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vg) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vg, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutg = true;
                        }
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vd) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vd, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutd = true;
                        }
                        /*On renvoie juste la variable*/
                        bilia_dterm_t * dtg = bilia_vector_at(dform->p.targs, 0);
                        bilia_dterm_t * dtd = bilia_vector_at(dform->p.targs, 1);
                        bilia_vector_at(dform->p.targs, 0) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT);
                        bilia_vector_at(dform->p.targs, 1) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT);
                        dform->kind = BILIA_DATA_GT;
                        dform->typ = BILIA_TYP_INT;
                        if (ajoutd)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMAX;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtd);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        if (ajoutg)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMIN;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtg);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(vg);
                        free(vd);
                }
                break;
        case BILIA_DATA_LE:
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT) break;
                {
                        assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                        char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                        char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                        bool ajoutd = false;
                        bool ajoutg = false;
                        char * vg = malloc(strlen(v0) + strlen("bagmax") + 1);
                        char * vd = malloc(strlen(v1) + strlen("bagmin") + 1);
                        sprintf(vg, "bagmax%s", v0);
                        sprintf(vd, "bagmin%s", v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vg) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vg, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutg = true;
                        }
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vd) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vd, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutd = true;
                        }
                        /*On renvoie juste la variable*/
                        bilia_dterm_t * dtg = bilia_vector_at(dform->p.targs, 0);
                        bilia_dterm_t * dtd = bilia_vector_at(dform->p.targs, 1);
                        bilia_vector_at(dform->p.targs, 0) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT);
                        bilia_vector_at(dform->p.targs, 1) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT);
                        dform->kind = BILIA_DATA_LE;
                        dform->typ = BILIA_TYP_INT;
                        if (ajoutd)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMIN;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtd);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        if (ajoutg)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMAX;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtg);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(vg);
                        free(vd);
                }
                break;
        case BILIA_DATA_LT:
                if (bilia_vector_at(dform->p.targs, 1)->typ != BILIA_TYP_BAGINT) break;
                {
                        assert(bilia_vector_at(dform->p.targs, 0)->kind == BILIA_DATA_VAR);
                        assert(bilia_vector_at(dform->p.targs, 1)->kind == BILIA_DATA_VAR);
                        char * v0 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 0)->p.sid, BILIA_TYP_BAGINT);
                        char * v1 = bilia_var_name(bilia_entl_get_var_array(), bilia_vector_at(dform->p.targs, 1)->p.sid, BILIA_TYP_BAGINT);
                        bool ajoutd = false;
                        bool ajoutg = false;
                        char * vg = malloc(strlen(v0) + strlen("bagmax") + 1);
                        char * vd = malloc(strlen(v1) + strlen("bagmin") + 1);
                        sprintf(vg, "bagmax%s", v0);
                        sprintf(vd, "bagmin%s", v1);
                        /*Aprés application recursive du truc tout mes fils sont censé etre des variable*/
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vg) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vg, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutg = true;
                        }
                        if (bilia_var_array_find_local(bilia_entl_get_var_array(), vd) == UNDEFINED_ID)
                        {
                                /*CREATION*/
                                /*On ajoute a rform v = v0 BAGMINUS v1*/
                                bilia_var_register (bilia_entl_get_var_array(), vd, tyint, BILIA_SCOPE_GLOBAL,true);
                                ajoutd = true;
                        }
                        /*On renvoie juste la variable*/
                        bilia_dterm_t * dtg = bilia_vector_at(dform->p.targs, 0);
                        bilia_dterm_t * dtd = bilia_vector_at(dform->p.targs, 1);
                        bilia_vector_at(dform->p.targs, 0) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT);
                        bilia_vector_at(dform->p.targs, 1) = bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT);
                        dform->kind = BILIA_DATA_LT;
                        dform->typ = BILIA_TYP_INT;
                        if (ajoutd)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMIN;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtd);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vd), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        if (ajoutg)
                        {
          #ifdef BENCH
                                nb_constraint_before++;
          #endif
                                bilia_dterm_t * nt = bilia_dterm_new();
                                nt->kind = BILIA_DATA_BAGMAX;
                                nt->typ = BILIA_TYP_INT;
                                nt->args = bilia_dterm_array_new();
                                bilia_dterm_array_push (nt->args, dtg);
                                bilia_dform_t * tmp = bilia_dform_new_eq (bilia_dterm_new_var(bilia_var_array_find_local(bilia_entl_get_var_array(), vg), BILIA_TYP_INT), nt);
                                bilia_dform_array_push(rarray, tmp);
                        }
                        free(vg);
                        free(vd);
                }
                break;
                /*x1neqx2*/
        }
}


void bilia_rewrite()
{

        for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_nform()); i++)
                bilia_create_nvar(bilia_vector_at(bilia_entl_get_nform(), i),bilia_entl_get_rform());
        //while(kill_pos_implies()+kill_neg_implies()+kill_rew_implies() != 0);}
        if (bilia_option_get_verb () > 0)
        {
                fprintf(stdout, "\n=================================================\n");
                bilia_entl_fprint(stdout);
                fflush(stdout);
        }
}
