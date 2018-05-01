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
 * Abstract Syntax Tree of BILIA formulas.
 */

#ifndef _BILIA_FORM_H_
#define	_BILIA_FORM_H_

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "bilia_vars.h"
#include "bilia_types.h"

/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

/** Theory of formulas.
 */
  typedef enum bilia_logic_t
  {
    BILIA_LOGIC_BILIA,            /* ESOP'13 */
    BILIA_LOGIC_SLL,             /* APLAS'14 */
    BILIA_LOGIC_SLRD,            /* SLCOMP'14 */
    BILIA_LOGIC_SLRDI,
    BILIA_LOGIC_OTHER            /* NOT SUPPORTED */
  } bilia_logic_t;

/** Data formulas.
 *  Encoded by smtlib expressions in bilia.h
 */
  typedef enum bilia_data_op_t
  {
    BILIA_DATA_INT = 0,
    BILIA_DATA_AND,
    BILIA_DATA_OR,
    BILIA_DATA_NOT,
    BILIA_DATA_IMPLIES,
    BILIA_DATA_VAR,
    BILIA_DATA_EMPTYBAG,
    BILIA_DATA_EQ,
    BILIA_DATA_NEQ,
    // BILIA_DATA_BAGEQ,
    // BILIA_DATA_BAGNEQ,
    BILIA_DATA_ITE,
    BILIA_DATA_LT,
    // BILIA_DATA_BAGLT,
    BILIA_DATA_GT,
    // BILIA_DATA_BAGGT,
    BILIA_DATA_LE,
    // BILIA_DATA_BAGLE,
    BILIA_DATA_GE,
    // BILIA_DATA_BAGGE,
    BILIA_DATA_PLUS,
    BILIA_DATA_MINUS,
    BILIA_DATA_BAG,
    BILIA_DATA_BAGN,
    BILIA_DATA_BAGUNION,
    BILIA_DATA_BAGMINUS,
    BILIA_DATA_BAGINTER,
    BILIA_DATA_BAGSUM,
    BILIA_DATA_MAX,
    BILIA_DATA_BAGMAX,
    BILIA_DATA_MIN,
    BILIA_DATA_BAGMIN,
    BILIA_DATA_IN,
    BILIA_DATA_INN,
    BILIA_DATA_NIN,
    BILIA_DATA_NI,
    BILIA_DATA_SUBSETEQ,
    BILIA_DATA_NSUBSETEQ,
    BILIA_DATA_APP,
    BILIA_DATA_OTHER             /* NOT TO BE USED */
  } bilia_data_op_t;

  typedef struct bilia_dterm_s bilia_dterm_t;     /* forward definition */
    BILIA_VECTOR_DECLARE (bilia_dterm_array, bilia_dterm_t *);

  typedef struct bilia_dform_s bilia_dform_t;     /* forward definition */
    BILIA_VECTOR_DECLARE (bilia_dform_array, bilia_dform_t *);

  struct bilia_dterm_s
  {
    bilia_data_op_t kind;        // only data terms
    bilia_typ_t typ;             // either BILIA_TYP_INT or BILIA_TYP_BAGINT

    union
    {
      long value;               // integer constant
      uid_t sid;                // symbol (variable or field) identifier
      bilia_dform_t *cond;       // simple condition for ite
    } p;

    bilia_dterm_array *args;     // NULL for 0-arity terms
  };


  struct bilia_dform_s
  {
    bilia_data_op_t kind;        // only data formulas
    bilia_typ_t typ;             // either BILIA_TYP_INT or BILIA_TYP_BAGINT

    union
    {
      bilia_dterm_array *targs;  // term arguments
      bilia_dform_array *bargs;  // boolean arguments iff kind == BILIA_DATA_IMPLIES
    } p;
  };


  typedef enum bilia_form_kind_t
  {
    BILIA_FORM_UNSAT = 0, BILIA_FORM_SAT, BILIA_FORM_VALID, BILIA_FORM_OTHER
/* NOT TO BE USED */
  } bilia_form_kind_t;

/* ====================================================================== */
/* Globals */
/* ====================================================================== */

  extern bilia_logic_t bilia_form_logic;

/* ====================================================================== */
/* Constructors/destructors */
/* ====================================================================== */

  // bilia_form_t *bilia_form_new (void);
  // bilia_pure_t *bilia_pure_new (uint_t size);

  bilia_dterm_t *bilia_dterm_new (void);
  bilia_dterm_t *bilia_dterm_copy (bilia_dterm_t *);
  bilia_dterm_t *bilia_dterm_new_var (uint_t vid, bilia_typ_t ty);
  bilia_dform_t *bilia_dform_new (void);
  bilia_dform_t *bilia_dform_copy (bilia_dform_t *);
  bilia_dform_t *bilia_dform_new_eq (bilia_dterm_t * t1, bilia_dterm_t * t2);

/* Allocation */

  // void bilia_form_free (bilia_form_t * f);
  // void bilia_form_set_unsat (bilia_form_t * f);
  void bilia_dterm_free (bilia_dterm_t * t);
  void bilia_dform_free (bilia_dform_t * d);
  // void bilia_pure_free (bilia_pure_t * p);
  //
  // int bilia_form_type (bilia_form_t * form);
  /* Type the formula */

/* ====================================================================== */
/* Solvers */
/* ====================================================================== */

  // int bilia_pure_add_dform (bilia_pure_t * form, bilia_dform_t * df);
  // int bilia_pure_check_entl (bool ** diff, uint_t dsize, bilia_pure_t * f,
  //                           bilia_uid_array * args,
  //                           bilia_var_array * lv, bilia_uid_array * map,
  //                           bilia_dform_array * df);
/* Check that @p diff entails @p ops[@p map] */

  // int bilia_dform_array_check_entl (bilia_var_array * lv1,
  //                                  bilia_dform_array * df1,
  //                                  bilia_var_array * lv2, bilia_uid_array * m,
  //                                  bilia_dform_array * df2);
/* Check that constraints on data variables from @p df1 entail @p df2 */

/* ====================================================================== */
/* Printing */
/* ====================================================================== */


  //void bilia_pure_fprint (FILE * f, bilia_var_array * lvars, bilia_pure_t * phi);

  void bilia_dform_fprint (FILE * f, bilia_var_array * lvars,
                          bilia_dform_t * phi,int level);
  void bilia_dform_array_fprint (FILE * f, bilia_var_array * lvars,
                                bilia_dform_array * phi,int level);

  //void bilia_form_fprint (FILE * f, bilia_form_t * phi, bilia_var_array * lvars);
  void bilia_dterm_fprint (FILE * f, bilia_var_array * lvars, bilia_dterm_t * dt,int level);

  int bilia_constraint_costly_form(bilia_dform_t *);
  int bilia_constraint_costly_term(bilia_dterm_t *);


#ifdef	__cplusplus
}
#endif

#endif                          /* _BILIA_FORMULA_H_ */
