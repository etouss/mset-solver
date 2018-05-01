/**************************************************************************/
/*                                                                        */
/*  BaTS decision procedure                                               */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.                                                */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.                  */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/

/*
 * Sat checking and diagnosis
 */
 
#include "bilia_sat.h"
#include "bilia_entl.h"
#include "bilia_option.h"
#include "bilia_nnf.h"
#include "bilia2smt.h"
#include "bilia_step3.h"
#include "bilia_step2_UF.h"
#include "bilia_step3_UF.h"
#include "bilia_optim_bag.h"
#include "bilia_var_check.h"
#include "bilia_z3_api.h"

/* ====================================================================== */
/* Utilities */
/* ====================================================================== */

/**
 * compute the difference between two times.
 *
 * @return 1 if the difference is negative, otherwise 0.
 */
int
time_difference (struct timeval *result, struct timeval *t2,
                 struct timeval *t1)
{
  long int diff = (t2->tv_usec + 1000000 * t2->tv_sec)
                  - (t1->tv_usec + 1000000 * t1->tv_sec);
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;

  return (int) (diff < 0);
}

/* ====================================================================== */
/* Sat checking */
/* ====================================================================== */

/**
 * Type the predicates, fields, formulas in bilia_prob.
 * @return 1 if typing is ok
 */
// int
// bilia_sat_type (void)
// {
//   /*
//    * Type formulas inside the problem.
//    */
//   // if (bilia_form_type (bilia_prob->pform) == 0)
//   //   return 0;
//
//   /* for (uint_t i = 0; i < bilia_vector_size (bilia_prob->nform); i++) */
//   if (bilia_form_type (bilia_entl_get_nform_last()) == 0)
//   {
// #ifndef NDEBUG
//     fprintf (stdout, "*** bilia_entl_type: type error in nform.\n");
//     fflush (stdout);
// #endif
//     return 0;
//   }
//
//   return 1;
// }

/**
 * Free memory allocated for sat checking
 */
// void
// bilia_sat_free_aux (bilia_form_t * form)
// {
//   assert (bilia_prob != NULL);
//   assert (bilia_prob->pform == form);
//
//   if (form != form)
//     assert (0);                 // to remove warning on unused parameter
// }

int
bilia_sat_solve ()
{
  /* check that form is exactly the positive formula */
  /* assert (bilia_prob->pform == form); */

  /*
   * Special case: null or unsat input formula
   */
  // if (bilia_prob->pform == NULL || bilia_prob->pform->kind == BILIA_FORM_UNSAT)
  // {
  //   if (bilia_option_get_verb () > 0)
  //     fprintf (stdout, " unsat formula!\n");
  //   return 0;
  // }

  struct timeval tvBegin, tvEnd, tvDiff;

  gettimeofday (&tvBegin, NULL);

  /*
   * Compute typing infos
   */
  if (bilia_option_get_verb () > 0)
    fprintf (stdout, "  > typing formula\n");

  // bilia_sat_type ();

#ifndef NDEBUG
  fprintf (stdout, "\n*** bilia_sat_solve: after typing problem:\n");
  /* bilia_fields_array_fprint (stdout, "fields:"); */
  fflush (stdout);
#endif

extern char* file_name;

#ifdef BENCH
FILE *f = fopen ("benchmark.txt", "a");
fprintf(f,"begin\n");
fprintf(f,"file:%s\n",file_name);
fprintf(f, "option:");

if(bilia_option_get_cvc4())
  fprintf(f,"cvc4");
else
  fprintf(f,"z3");

if(bilia_option_get_optim_bag())
  fprintf(f,",bag");

if(bilia_option_get_optim_ele())
  fprintf(f,",ele");
if(bilia_option_get_nr())
  fprintf(f,",nr");

if(bilia_option_get_uinterpreted())
  fprintf(f,",QFUFLIA");
else
  fprintf(f,",QFLIA");
fprintf(f,"\n");

int bag_var = 0;
int int_var = 0;
for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_var_array()); i++)
  if(bilia_vector_at(bilia_entl_get_var_array(),i)->vty->kind == BILIA_TYP_INT)
    int_var ++;
  else
    bag_var ++;

//int bag_constraint = 0;
fprintf(f,"before\n");
fprintf(f,"bag_var:%d\n",bag_var);
fprintf(f,"int_var:%d\n",int_var);
#endif


    bilia_nnf();
    // bilia_entl_fprint(stdout);
    if(bilia_option_get_optim_bag())
      bilia_optim_bag();
    bilia_rewrite();

#ifdef BENCH
extern int nb_constraint_before;
fprintf(f,"bag_constraints:%d\n",nb_constraint_before);
#endif

    bilia_var_check();
    if (!bilia_option_get_uinterpreted())
    {
      bilia_step2();
      bilia_step3();
    }
    else{
      bilia_step2_uf();
      bilia_step3_uf();
    }

#ifdef BENCH
int bag_var_af = 0;
int int_var_af = 0;
int int_var_ele = 0;
for (uint_t i = 0; i < BILIA_VECTOR_SIZE(bilia_entl_get_var_array()); i++)
  if(bilia_vector_at(bilia_entl_get_var_array(),i)->vty->kind == BILIA_TYP_INT){
    int_var_af ++;
    if(bilia_vector_at(bilia_entl_get_var_array(),i)->element)
      int_var_ele ++;
  }
  else
    bag_var_af ++;
fprintf(f,"after\n");
fprintf(f,"bag_var:%d\n",bag_var_af);
fprintf(f,"int_var:%d\n",int_var_af);
if(bilia_option_get_optim_ele())
  fprintf(f,"int_var_ele:%d\n",int_var_ele);
else
  fprintf(f,"int_var_ele:0\n");
#endif

    //bilia_z3_api();

    bilia2smt();

    gettimeofday (&tvEnd, NULL);
    time_difference (&tvDiff, &tvEnd, &tvBegin);
    printf ("My time (sec): %ld.%06ld\n\n", (long int) tvDiff.tv_sec,
            (long int) tvDiff.tv_usec);


    #ifdef BENCH
    fprintf(f,"time1:%ld.%06ld\n", (long int) tvDiff.tv_sec,
            (long int) tvDiff.tv_usec);
    fflush(f);
    #endif

    char * name = malloc(strlen("../smt/")+strlen(file_name)+1);
    sprintf(name,"../smt/%s",file_name);
    char *command = (char *) malloc (100 * sizeof (char));
    memset (command, 0, 100 * sizeof (char));
    #ifndef BENCH
    if(!bilia_option_get_cvc4())
    sprintf (command, "/usr/local/bin/z3 -smt2 %s ",name);
    else
      sprintf (command, "/usr/local/bin/cvc4 --lang smt %s ",name);
    #endif
    #ifdef BENCH
    if(!bilia_option_get_cvc4())
    sprintf (command, "/usr/local/bin/z3 -smt2 %s >>benchmark.txt ",name);
    else
      sprintf (command, "/usr/local/bin/cvc4 --lang smt >>benchmark.txt %s ",name);
    #endif



    system(command);
    free(command);
    free(name);

  /* bilia_prob->pabstr = bilia_normalize (form, "f-out.txt", true, false); */

  /*
   * FIN
   */

  gettimeofday (&tvEnd, NULL);
  time_difference (&tvDiff, &tvEnd, &tvBegin);
  printf ("Total time (sec): %ld.%06ld\n\n", (long int) tvDiff.tv_sec,
         (long int) tvDiff.tv_usec);

   #ifdef BENCH
   fprintf(f,"time2:%ld.%06ld\n", (long int) tvDiff.tv_sec,
           (long int) tvDiff.tv_usec);
   fprintf(f,"end\n\n");
   fclose(f);
   #endif

  /*
   * Free the allocated memory
   * (only boolean abstraction, formulas will be deallocated at the end)
   */
  //bilia_sat_free_aux (bilia_prob->pform);
  return 0;
}
