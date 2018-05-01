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
 * NNF.
 */

#include "bilia_nnf.h"

/* Suppress Implies and ITE in the formula */
void bilia_implies_sup(bilia_dform_t * form,bilia_dterm_t * term,bilia_dform_array * fa){
  if(form != NULL){
    if(form->kind == BILIA_DATA_AND || form->kind == BILIA_DATA_OR || form->kind == BILIA_DATA_IMPLIES || form->kind == BILIA_DATA_NOT)
      for (uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.bargs) ; i++)
            bilia_implies_sup(bilia_vector_at(form->p.bargs,i),NULL,form->p.bargs);
    if(form->kind == BILIA_DATA_EQ || form->kind == BILIA_DATA_NEQ || form->kind == BILIA_DATA_LE || form->kind == BILIA_DATA_GE || form->kind == BILIA_DATA_GT \
     || form->kind == BILIA_DATA_LT || form->kind == BILIA_DATA_INN  || form->kind == BILIA_DATA_NIN  || form->kind == BILIA_DATA_IN  || form->kind == BILIA_DATA_NI \
      || form->kind == BILIA_DATA_SUBSETEQ || form->kind == BILIA_DATA_NSUBSETEQ)
      /*Here we try to know if we have to deal with ite*/
      for (uint_t i = 0; i < BILIA_VECTOR_SIZE(form->p.targs) ; i++){
            bilia_implies_sup(NULL,bilia_vector_at(form->p.targs,i),fa);
            if(bilia_vector_at(form->p.targs,i)->kind == BILIA_DATA_ITE){
              /*Here we have to duplicate the last literal in order to create the 2 possiblites*/
              bilia_dterm_t * ite = bilia_dterm_copy(bilia_vector_at(form->p.targs,i));

              bilia_dform_t * nfp = bilia_dform_new();
              nfp->typ = form->typ;
              nfp->kind = form->kind;
              nfp->p.targs = bilia_dterm_array_new();
              for (uint_t j = 0; j < BILIA_VECTOR_SIZE(form->p.targs) ; j++)
                if(i!=j)
                  bilia_dterm_array_push(nfp->p.targs,bilia_dterm_copy(bilia_vector_at(form->p.targs,j)));
                else
                  bilia_dterm_array_push(nfp->p.targs,bilia_dterm_copy(bilia_vector_at(ite->args,1)));
              bilia_dform_t * nfn = bilia_dform_new();
              nfn->typ = form->typ;
              nfn->kind = form->kind;
              nfn->p.targs = bilia_dterm_array_new();
              for (uint_t j = 0; j < BILIA_VECTOR_SIZE(form->p.targs) ; j++)
                if(i!=j)
                  bilia_dterm_array_push(nfn->p.targs,bilia_dterm_copy(bilia_vector_at(form->p.targs,j)));
                else
                  bilia_dterm_array_push(nfn->p.targs,bilia_dterm_copy(bilia_vector_at(ite->args,0)));
             form->kind = BILIA_DATA_OR;

             for (uint_t j = 0; j < BILIA_VECTOR_SIZE(form->p.targs) ; j++)
                 bilia_dterm_free(bilia_vector_at(form->p.targs,j));
             bilia_dterm_array_delete(form->p.targs);

             form->p.bargs = bilia_dform_array_new();
             bilia_dform_array_push(form->p.bargs,bilia_dform_copy(ite->p.cond));
             bilia_dform_array_push(form->p.bargs,nfp);

             bilia_dform_t * ncond = bilia_dform_new();
             ncond->typ = BILIA_TYP_BOOL;
             ncond->kind = BILIA_DATA_NOT;
             ncond->p.bargs = bilia_dform_array_new();
             bilia_dform_array_push(ncond->p.bargs,bilia_dform_copy(ite->p.cond));

             bilia_dform_t * nform = bilia_dform_new();
             nform->typ = BILIA_TYP_BOOL;
             nform->kind = BILIA_DATA_OR;
             nform->p.bargs = bilia_dform_array_new();
             bilia_dform_array_push(nform->p.bargs,ncond);
             bilia_dform_array_push(nform->p.bargs,nfn);
             /*We add the 2nd possibilites to the last Formula encounter (Maybe an issue have to check it)*/
             /*Can create and otherwise safer.*/
             bilia_dform_array_push(fa,nform);
            }
          }
    if(form->kind == BILIA_DATA_IMPLIES){
      /*Replace a => b with not a or b*/
      bilia_dform_t * neg = bilia_dform_new();
      neg->kind = BILIA_DATA_NOT;
      neg->typ = BILIA_TYP_BOOL;
      neg->p.bargs = bilia_dform_array_new();
      bilia_dform_array_push(neg->p.bargs,bilia_vector_at(form->p.bargs,0));
      form->kind = BILIA_DATA_OR;
      //bilia_dform_free(bilia_vector_at(form->p.bargs,0));
      bilia_vector_at(form->p.bargs,0) = neg;
    }
  }
  else if(term!=NULL){
    if(term->args != NULL){
      for (uint_t i = 0; i < BILIA_VECTOR_SIZE(term->args) ; i++)
            bilia_implies_sup(NULL,bilia_vector_at(term->args,i),fa);
    }
  }
}

/*Modify the formula by pushing every neg as deep as possible*/
void bilia_nnf_apply(bilia_dform_t * form,bool neg,bilia_dform_t **pf){
  if(neg){
    /*Simple negation of every literal*/
    switch(form->kind){
      case BILIA_DATA_LT:
        form->kind = BILIA_DATA_GE;
        break;
      case BILIA_DATA_GT:
        form->kind = BILIA_DATA_LE;
        break;
      case BILIA_DATA_LE:
        form->kind = BILIA_DATA_GT;
        break;
      case BILIA_DATA_GE:
        form->kind = BILIA_DATA_LT;
        break;
      case BILIA_DATA_EQ:
        form->kind = BILIA_DATA_NEQ;
        break;
      case BILIA_DATA_NEQ:
        form->kind = BILIA_DATA_EQ;
        break;
      case BILIA_DATA_SUBSETEQ:
        form->kind = BILIA_DATA_NSUBSETEQ;
        break;
      case BILIA_DATA_NSUBSETEQ:
        form->kind = BILIA_DATA_SUBSETEQ;
        break;
      case BILIA_DATA_IN:
        form->kind = BILIA_DATA_NI;
        break;
      case BILIA_DATA_NI:
        form->kind = BILIA_DATA_IN;
        break;
      case BILIA_DATA_INN:
        form->kind = BILIA_DATA_NIN;
        break;
      case BILIA_DATA_NIN:
        form->kind = BILIA_DATA_INN;
        break;
      /*Morgan application*/
      case BILIA_DATA_AND:
        form->kind = BILIA_DATA_OR;
        for (uint_t i = 0;  i< BILIA_VECTOR_SIZE(form->p.bargs); i++)
          bilia_nnf_apply(bilia_vector_at(form->p.bargs,i),neg,&bilia_vector_at(form->p.bargs,i));
        break;
      case BILIA_DATA_OR:
        form->kind = BILIA_DATA_AND;
        for (uint_t i = 0;  i< BILIA_VECTOR_SIZE(form->p.bargs); i++)
          bilia_nnf_apply(bilia_vector_at(form->p.bargs,i),neg,&bilia_vector_at(form->p.bargs,i));
        break;
      /*Cancel the neg*/
      case BILIA_DATA_NOT:
        bilia_nnf_apply(bilia_vector_at(form->p.bargs,0),!neg,&bilia_vector_at(form->p.bargs,0));
        *pf = bilia_vector_at(form->p.bargs,0);
        break;
      default:
        exit(6);
    }
  }
  else if(form->kind == BILIA_DATA_NOT){
    /*Init the neg*/
    bilia_nnf_apply(bilia_vector_at(form->p.bargs,0),!neg,&bilia_vector_at(form->p.bargs,0));
    *pf = bilia_vector_at(form->p.bargs,0);
    bilia_dform_array_delete(form->p.bargs);
    free(form);
  }
  else if(form->kind == BILIA_DATA_AND || form->kind == BILIA_DATA_OR){
    /*Propagate the nnf*/
    for (uint_t i = 0;  i< BILIA_VECTOR_SIZE(form->p.bargs); i++)
      bilia_nnf_apply(bilia_vector_at(form->p.bargs,i),neg,&bilia_vector_at(form->p.bargs,0));
  }
}

void bilia_nnf(){
  /*Probleme avec le premier af*/
  bilia_dform_array * af = bilia_entl_get_nform();
  for (size_t i = 0; i < BILIA_VECTOR_SIZE(af); i++)
    bilia_implies_sup(bilia_vector_at(af,i),NULL,af);
  for (size_t i = 0; i < BILIA_VECTOR_SIZE(af); i++)
    bilia_nnf_apply(bilia_vector_at(af,i),false,&bilia_vector_at(af,i));
}
